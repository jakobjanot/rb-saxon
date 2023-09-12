#include "XdmValue.h"
#include "XdmArray.h"
#include "XdmAtomicValue.h"
#include "XdmFunctionItem.h"
#include "XdmItem.h"
#include "XdmMap.h"
#include "XdmNode.h"

#ifdef MEM_DEBUG
#define new new (__FILE__, __LINE__)
#endif

XdmValue::XdmValue(const XdmValue &other) {
  // SaxonProcessor *proc = other.proc; //TODO
  valueType = other.valueType;
  refCount = 0;
  xdmSize = other.xdmSize;
  jValues = other.jValues;
  toStringValue = other.toStringValue;
  values.resize(0);
  for (int i = 0; i < xdmSize; i++) {
    addXdmItem(other.values[i]);
  }
}

const char *XdmValue::toString() {
  int count = size();
  if (count == 0) {
    return nullptr;
  }
  if (toStringValue == nullptr) {
    allocFn fn = operator new;
    if (count == 1) {
      toStringValue =
          xdmItemToString(SaxonProcessor::sxn_environ->thread, (void *)fn,
                          (void *)(values[0]->getUnderlyingValue()));
    } else {
      if (jValues == -1) {
        jValues = getUnderlyingValue();
      }

      toStringValue = xdmValueArrayToString(SaxonProcessor::sxn_environ->thread,
                                            (void *)fn, (void *)jValues);
    }
  }
  return toStringValue;
}

int XdmValue::size() { return xdmSize; }

XdmValue::XdmValue(int64_t valRef) {
  XdmItem *value = new XdmItem(valRef);
  values.resize(
      0); // TODO memory issue might occur here. Need to delete XdmItems
  values.push_back(value);
  xdmSize++;
  jValues = -1;
  valueType = nullptr;
  toStringValue = nullptr;
}

XdmValue::XdmValue(int64_t val, bool arr) {
  xdmSize = 0;
  values.resize(0);
  jValues = -1;
  valueType = nullptr;
  toStringValue = nullptr;
  allocFn fn = operator new;
  long long *results = makeArrayFromXdmValue2(
      SaxonProcessor::sxn_environ->thread, (void *)fn, (void *)val);
  if (results == nullptr) {
    return;
  }

  int xdmSizei = results[0];
  int typeRef = 0;

  for (int p = 1; p <= xdmSizei; ++p) {
    typeRef = j_getXdmObjectType(SaxonProcessor::sxn_environ->thread,
                                 (void *)results[p]);
    if (typeRef == -2) {
      std::cerr << "Exception thrown while creating XdmValue from array"
                << std::endl;
      return;
    }
    if (typeRef == 1) {

      addXdmItem(new XdmAtomicValue(results[p]));

    } else if (typeRef == 2) {

      addXdmItem(new XdmNode(results[p]));

    } else if (typeRef == 3) {
      addXdmItem(new XdmArray(results[p]));
    } else if (typeRef == 4) {
      addXdmItem(new XdmMap(results[p]));

    } else if (typeRef == 5) {
      addXdmItem(new XdmFunctionItem(results[p]));
    } else {
      std::cerr << "Error creating XdmValue from array" << std::endl;
    }
  }
  // delete results;
}

XdmValue::~XdmValue() {

  if (values.size() > 0) {
    for (size_t i = 0; i < values.size(); i++) {
      if (values[i] != nullptr && values[i]->getRefCount() < 1) {
        delete values[i];
      }
    }
    values.clear();
  }
  if (valueType != nullptr) {
    delete valueType;
  }
  if (jValues > 0 && getRefCount() < 1) {
    j_handles_destroy(SaxonProcessor::sxn_environ->thread, (void *)jValues);
    jValues = -1;
  }

  xdmSize = 0;

  if (toStringValue != nullptr) {
    delete[] toStringValue;
    toStringValue = nullptr;
  }
}

void XdmValue::addXdmItem(XdmItem *val) {
  if (val != nullptr) {
    values.push_back(val);
    val->incrementRefCount();
    xdmSize++;
    if (jValues >= 0) {
      jValues = -1;
    }
    if (toStringValue != nullptr) {
      delete[] toStringValue;
      toStringValue = nullptr;
    }
  }
}

void XdmValue::addUnderlyingValue(int64_t val) {
  XdmItem *valuei = new XdmItem(val);
  valuei->incrementRefCount();
  values.push_back(valuei);
  xdmSize++;
  jValues = -1; // TODO clear Graalvm reference from ObjectHandles pool
}

void XdmValue::incrementRefCount() {
  refCount++;
  if (getenv("SAXONC_DEBUG_FLAG")) {
    std::cerr << "refCount-inc-xdmVal=" << refCount << " ob ref=" << (this)
              << std::endl;
  }
}

void XdmValue::decrementRefCount() {
  if (refCount > 0)
    refCount--;
  if (getenv("SAXONC_DEBUG_FLAG")) {
    std::cerr << "refCount-dec-xdmVal=" << refCount << " ob ref=" << (this)
              << std::endl;
  }
}

XdmItem *XdmValue::getHead() {
  if (values.size() > 0) {
    return values[0];
  } else {
    return nullptr;
  }
}

int64_t XdmValue::getUnderlyingValue() {

  if (jValues == -1) {
    int i;

    int count = values.size();
    if (count == 0) {
      return -1;
    }

    if (count == 1) {
      return values[0]->getUnderlyingValue();
    }
    jValues = createProcessorDataWithCapacity(
        SaxonProcessor::sxn_environ->thread, count);

    for (i = 0; i < count; i++) {
      int64_t ivalue = values[i]->getUnderlyingValue();
      addProcessorValue(SaxonProcessor::sxn_environ->thread, (void *)jValues,
                        (void *)ivalue);
    }
  }
  return jValues;
}

void XdmValue::releaseXdmValue() {
  for (size_t i = 0; i < values.size(); i++) {
    if (values[i] != nullptr) {
      delete values[i];
    }
  }

  jValues = -1;
}

XdmItem *XdmValue::itemAt(int n) {
  if (n >= 0 && (unsigned int)n < values.size()) {
    return values[n];
  }
  return nullptr;
}

/**
 * Get the type of the object
 */
XDM_TYPE XdmValue::getType() { return XDM_VALUE; }
