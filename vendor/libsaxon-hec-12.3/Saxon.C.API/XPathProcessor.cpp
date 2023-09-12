#include "XPathProcessor.h"
#include "XdmArray.h"
#include "XdmAtomicValue.h"
#include "XdmFunctionItem.h"
#include "XdmItem.h"
#include "XdmMap.h"
#include "XdmNode.h"
#include "XdmValue.h"

#include <fstream>
#include <iostream>

#ifdef MEM_DEBUG
#define new new (__FILE__, __LINE__)
#endif

XPathProcessor::XPathProcessor() {
  SaxonProcessor *p = new SaxonProcessor(false);
  XPathProcessor(p, "");
}

XPathProcessor::XPathProcessor(SaxonProcessor *p, std::string curr) {
  proc = p;
  // SaxonProcessor::attachCurrentThread();

  /*cppClass = lookForClass(SaxonProcessor::sxn_environ->env,
                  "net/sf/saxon/option/cpp/XPathProcessor");
  if ((proc->proc) == nullptr) {
          std::cerr << "Processor is nullptr" << std::endl;
  }

jobject tempcppXP = createSaxonProcessor2(SaxonProcessor::sxn_environ->env,
cppClass,
                  "(Lnet/sf/saxon/s9api/Processor;)V", proc->proc);*/

  cppXP = createXPathProcessorWithProcessor(SaxonProcessor::sxn_environ->thread,
                                            (void *)p->procRef);

  if (cppXP == -1) {

    std::cerr << "Error: Failed to create the XPathProcessor internal object"
              << std::endl;

    return;
  }

#ifdef DEBUG
  /*jmethodID debugMID =
  SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass, "setDebugMode",
  "(Z)V"); SaxonProcessor::sxn_environ->env->CallStaticVoidMethod(cppClass,
  debugMID, (jboolean)true);*/
#endif

  exception = nullptr;
  if (!(proc->cwd.empty()) && curr.empty()) {
    cwdXP = proc->cwd;
  } else {
    cwdXP = curr;
  }
}

XPathProcessor::XPathProcessor(const XPathProcessor &other) {
  cwdXP = other.cwdXP;
  proc = other.proc; // TODO check thread safety

  cppXP = other.cppXP;
  std::map<std::string, XdmValue *>::const_iterator paramIter =
      other.parameters.begin();
  while (paramIter != other.parameters.end()) {

    XdmValue *valuei = paramIter->second;
    if (valuei == nullptr) {

    } else {
      parameters[paramIter->first] = new XdmValue(*(valuei));
    }
    paramIter++;
  }

  std::map<std::string, std::string>::const_iterator propIter =
      other.properties.begin();
  while (propIter != other.properties.end()) {
    properties[propIter->first] = propIter->second;
    propIter++;
  }
}

XPathProcessor::~XPathProcessor() {
  clearProperties();
  clearParameters();
  if (cppXP > 0) {
    j_handles_destroy(SaxonProcessor::sxn_environ->thread, (void *)cppXP);
  }
  cppXP = -1;
  // exceptionClear();
}

XdmValue *XPathProcessor::evaluate(const char *xpathStr) {
  if (xpathStr == nullptr) {
    throw SaxonApiException("Error:: XPath string cannot be empty or nullptr");
  }
  SaxonProcessor::attachCurrentThread();

  setProperty("resources", proc->getResourcesDirectory());
  int64_t processorDataRef =
      SaxonProcessor::createParameterJArray(parameters, properties, 0);
  int64_t results =
      j_evaluate(SaxonProcessor::sxn_environ->thread, (void *)cppXP,
                 (char *)cwdXP.c_str(), (char *)xpathStr,
                 (void *)(processorDataRef == -1 ? (void *)NULL
                                                 : (void *)processorDataRef));
  if (processorDataRef > 0) {
    j_handles_destroy(SaxonProcessor::sxn_environ->thread,
                      (void *)processorDataRef);
    processorDataRef = -1;
  }
  if (results > 0) {
    return SaxonProcessor::makeXdmValueFromRef(results);
  } else if (results == -2) {
    throw SaxonApiException();
  }

  return nullptr;
}

XdmItem *XPathProcessor::evaluateSingle(const char *xpathStr) {

  if (xpathStr == nullptr) {
    throw SaxonApiException("Error:: XPath string cannot be empty or nullptr");
  }
  SaxonProcessor::attachCurrentThread();

  setProperty("resources", proc->getResourcesDirectory());
  int64_t processorDataRef =
      SaxonProcessor::createParameterJArray(parameters, properties);
  int64_t result = j_evaluateSingle(
      SaxonProcessor::sxn_environ->thread, (void *)cppXP, (char *)cwdXP.c_str(),
      (char *)xpathStr,
      (void *)(processorDataRef == -1 ? (void *)NULL
                                      : (void *)processorDataRef));
  if (processorDataRef > 0) {
    j_handles_destroy(SaxonProcessor::sxn_environ->thread,
                      (void *)processorDataRef);
    processorDataRef = -1;
  }
  if (result > 0) {
    int typeRef =
        j_getXdmObjectType(SaxonProcessor::sxn_environ->thread, (void *)result);
    XdmItem *xdmItem = nullptr;

    if (typeRef == 1) {
      xdmItem = new XdmAtomicValue(result);
      return xdmItem;

    } else if (typeRef == 2) {
      xdmItem = new XdmNode(result);
      return xdmItem;

    } else if (typeRef == 4) {
      xdmItem = new XdmMap(result);
      return xdmItem;

    } else if (typeRef == 3) {
      xdmItem = new XdmArray(result);
      return xdmItem;

    } else if (typeRef == 5) {
      xdmItem = new XdmFunctionItem(result);
      return xdmItem;
    }
    xdmItem = new XdmItem(result);
    return xdmItem;
  } else if (result == -2) {

    throw SaxonApiException();
  }
  return nullptr;
}

void XPathProcessor::setContextItem(XdmItem *item) {
  if (item == nullptr) {
    std::map<std::string, XdmValue *>::iterator it;
    it = parameters.find("node");
    if (it != parameters.end()) {
      XdmValue *valuei = it->second;
      if (valuei != nullptr) {
        valuei->decrementRefCount();
        parameters.erase("node");
      }
    }
  } else {
    setParameter("node", item, false);
  }
}

void XPathProcessor::setContextFile(const char *filename) {

  if (filename != nullptr) {
    setProperty("s", filename);
  } else {
    properties.erase("s");
  }
}

/*void
XPathProcessor::setUnprefixedElementMatchingPolicy(UnprefixedElementMatchingPolicy
policy){ std::map<std::string, std::string>::iterator it; it =
properties.find("uemp:"); if (it != properties.end()) { std::string valuei =
it->second; parameters.erase("uemp:");
    }
    setProperty("uemp:", std::to_string(int(policy)).c_str());
}

UnprefixedElementMatchingPolicy
XPathProcessor::getUnprefixedElementMatchingPolicy()  { std::map<std::string,
std::string>::iterator it; it = properties.find("uemp:"); if (it !=
properties.end()) { int valuei = stoi(it->second); return
UnprefixedElementMatchingPolicy(valuei);
    }
    return UnprefixedElementMatchingPolicy::DEFAULT_NAMESPACE;
} */

void XPathProcessor::declareNamespace(const char *prefix, const char *uri) {
  SaxonProcessor::attachCurrentThread();
  if (prefix == nullptr) {
    throw SaxonApiException("Null prefix supplied to declareNamespace()");
  }
  if (uri == nullptr) {
    throw SaxonApiException(
        "Null namespace URI supplied to declareNamespace()");
  }
  j_xp_declareNamespace(SaxonProcessor::sxn_environ->thread, (void *)cppXP,
                        (char *)prefix, (char *)uri);
}

void XPathProcessor::declareVariable(const char *name) {
  if (name == nullptr) {
    return;
  }
  j_xp_declareVariable(SaxonProcessor::sxn_environ->thread, (void *)cppXP,
                       (char *)name);
}

#if CVERSION_API_NO >= 121

void XPathProcessor::setBackwardsCompatible(bool option) {
  if (option) {
    setProperty("backwardsCom:", "true");
  } else {
    properties.erase("backwardsCom:");
  }
}

void XPathProcessor::setLanguageVersion(const char *version) {
  if (version) {
    setProperty("lang:", version);
  } else {
    properties.erase("lang:");
  }
}

void XPathProcessor::setCaching(bool caching) {
  if (caching) {
    setProperty("caching:", "true");
  } else {
    properties.erase("caching:");
  }
}

void XPathProcessor::importSchemaNamespace(const char *uri) {
  if (uri != nullptr) {
    setProperty("importSN", uri);
  }
}

#endif

void XPathProcessor::setBaseURI(const char *uriStr) {
  j_xp_setBaseURI(SaxonProcessor::sxn_environ->thread, (void *)cppXP,
                  (char *)uriStr);
}

bool XPathProcessor::effectiveBooleanValue(const char *xpathStr) {
  SaxonProcessor::attachCurrentThread();
  setProperty("resources", proc->getResourcesDirectory());
  int64_t processorDataRef =
      SaxonProcessor::createParameterJArray(parameters, properties);
  int resultsi = j_effectiveBooleanValue(
      SaxonProcessor::sxn_environ->thread, (void *)cppXP, (char *)cwdXP.c_str(),
      (char *)xpathStr,
      (void *)(processorDataRef == -1 ? (void *)NULL
                                      : (void *)processorDataRef));
  if (processorDataRef > 0) {
    j_handles_destroy(SaxonProcessor::sxn_environ->thread,
                      (void *)processorDataRef);
    processorDataRef = -1;
  }
  if (resultsi == -2) {
    throw SaxonApiException();
  }

  return (bool)resultsi;
}

void XPathProcessor::setParameter(const char *name, XdmValue *value,
                                  bool withParam) {
  if (value != nullptr && name != nullptr) {
    std::string skey = (withParam ? "param:" : "") + std::string(name);
    std::map<std::string, XdmValue *>::iterator it = parameters.find(skey);
    if (it != parameters.end()) {
      XdmValue *valuei = it->second;
      if (valuei != nullptr) {
        valuei->decrementRefCount();
        parameters.erase(skey);
      }
    }
    parameters[skey] = value;
  }
}

void XPathProcessor::setParameter(const char *name, XdmValue *value) {
  setParameter(name, value, true);
}

bool XPathProcessor::removeParameter(const char *name) {
  return (bool)(parameters.erase("param:" + std::string(name)));
}

void XPathProcessor::setProperty(const char *name, const char *value) {
  if (name != nullptr) {
    int s = properties.size();
    std::string skey = std::string(name);
    properties.insert(std::pair<std::string, std::string>(
        skey, std::string((value == nullptr ? "" : value))));

    if (s == properties.size()) {
      std::map<std::string, std::string>::iterator it;
      it = properties.find(skey);
      if (it != properties.end()) {
        properties.erase(skey);
        properties[skey] = std::string((value == nullptr ? "" : value));
      }
    }
  }
}

void XPathProcessor::clearParameters(bool delVal) {
  // std::cerr<<"XPath clearParameter"<<std::endl;
  if (delVal) {
    for (std::map<std::string, XdmValue *>::iterator itr = parameters.begin();
         itr != parameters.end(); itr++) {
      XdmValue *value = itr->second;
      if (value != nullptr) {
#ifdef DEBUG
        std::cerr << "XPathProc.clearParameter() - XdmValue refCount="
                  << value->getRefCount() << std::endl;
#endif
        delete value;
      }
    }
  }
  parameters.clear();
}

void XPathProcessor::clearProperties() { properties.clear(); }

void XPathProcessor::setcwd(const char *dir) { cwdXP = std::string(dir); }

std::map<std::string, XdmValue *> &XPathProcessor::getParameters() {
  std::map<std::string, XdmValue *> &ptr = parameters;
  return ptr;
}

std::map<std::string, std::string> &XPathProcessor::getProperties() {
  std::map<std::string, std::string> &ptr = properties;
  return ptr;
}

void XPathProcessor::exceptionClear() {

  if (exception != nullptr) {
    delete exception;
    exception = nullptr;
  }
  j_clearException(SaxonProcessor::sxn_environ->thread);
}

const char *XPathProcessor::getErrorCode() {
  if (exception == nullptr) {
    return nullptr;
  }
  return exception->getErrorCode();
}

const char *XPathProcessor::getErrorMessage() {
  if (exception == nullptr) {
    return nullptr;
  }
  return exception->getMessage();
}

SaxonApiException *XPathProcessor::getException() { return exception; }

bool XPathProcessor::exceptionOccurred() {
  return proc->exceptionOccurred() || exception != nullptr;
}

void XPathProcessor::createException(const char *message) {
  if (exception != nullptr) {
    delete exception;
    exception = nullptr;
  }
  if (message == nullptr) {
    // exception = proc->checkAndCreateException(cppClass);
  } else {
    exception = new SaxonApiException(message);
  }
}
