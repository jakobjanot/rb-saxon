
//

#include "XdmFunctionItem.h"

#include "XdmArray.h"
#include "XdmMap.h"

#ifdef MEM_DEBUG
#define new new (__FILE__, __LINE__)
#endif

XdmFunctionItem::XdmFunctionItem() : XdmItem(), arity(-1), fname(nullptr) {}

XdmFunctionItem::XdmFunctionItem(const XdmFunctionItem &aVal) : XdmItem(aVal) {
  arity = aVal.arity;
  fname = aVal.fname;
}

XdmFunctionItem::XdmFunctionItem(int64_t obj)
    : XdmItem(obj), arity(-1), fname(nullptr) {}

XdmValue *XdmFunctionItem::getXdmValueSubClass(int64_t results) {
  if (results > 0) {
    XdmValue *value = nullptr;
    int typeRef = j_getXdmObjectType(SaxonProcessor::sxn_environ->thread,
                                     (void *)results);
    XdmItem *xdmItem = nullptr;
    if (typeRef == -2) {
      // TODO handle exception = new SaxonApiException();
      return nullptr;
    }
    if (typeRef == 1) {
      value = new XdmValue();
      xdmItem = new XdmAtomicValue(results);
      value->addXdmItem(xdmItem);

    } else if (typeRef == 2) {
      value = new XdmValue();
      xdmItem = new XdmNode(results);
      value->addXdmItem(xdmItem);

    } else if (typeRef == 4) {
      value = new XdmValue();
      xdmItem = new XdmMap(results);
      value->addXdmItem(xdmItem);

    } else if (typeRef == 3) {
      value = new XdmValue();
      xdmItem = new XdmArray(results);
      value->addXdmItem(xdmItem);

    } else if (typeRef == 5) {
      value = new XdmValue();
      xdmItem = new XdmFunctionItem(results);
      value->addXdmItem(xdmItem);
    } else if (typeRef == 6) {
      // Empty Sequence detected.
      // TODO Maybe we should create an EmptySequence object
      j_handles_destroy(SaxonProcessor::sxn_environ->thread, (void *)results);
      results = -1;
      return nullptr;

    } else {
      value = new XdmValue(results, true);
    }

    return value;
  } else if (results == -2) {
    // TODO exception = new SaxonApiException();
  }

  return nullptr;
}

const char *XdmFunctionItem::getName() {
  if (fname == nullptr) {
    allocFn fn = operator new;
    fname = j_getFunctionName(SaxonProcessor::sxn_environ->thread, (void *)fn,
                              (void *)value);
    return fname;
  } else {
    return fname;
  }
}

int XdmFunctionItem::getArity() {
  if (arity == -1) {
    if (value <= 0) {
      return -1;
    }
    arity = j_xdmFunctionItem_getArity(SaxonProcessor::sxn_environ->thread,
                                       (void *)value);
    return arity;
  } else {
    return arity;
  }
}

XdmFunctionItem *XdmFunctionItem::getSystemFunction(SaxonProcessor *processor,
                                                    const char *name,
                                                    int arity) {
  if (processor == nullptr || name == nullptr) {
    throw SaxonApiException("Error in getSystemFunction. Please make sure processor and "
                 "name are not nullptr.");
  }

  int64_t result =
      j_getSystemFunction(SaxonProcessor::sxn_environ->thread,
                          (void *)processor->procRef, (char *)name, arity);
  if (result >= 0) {
    XdmFunctionItem *functionItem = new XdmFunctionItem(result);
    return functionItem;
  }

  return nullptr;
}

XdmValue *XdmFunctionItem::call(SaxonProcessor *processor, XdmValue **arguments,
                                int argument_length) {
  if ((argument_length > 0 && arguments == nullptr) || processor == nullptr) {
    throw SaxonApiException(
        "Error in XdmFunctionItem.call.  nullptr arguments found.");
  }

  int64_t argumentJArrayRef =
      SaxonProcessor::createJArray(arguments, argument_length);
  if (argumentJArrayRef == -1 && argument_length > 0) {
    throw SaxonApiException("Error in XdmFunctionItem.call when converting "
                            "arguments -   nullptr arguments found.\"");
  }
  int64_t results = j_xdmFunctionItem_call(
      SaxonProcessor::sxn_environ->thread, (void *)processor->procRef,
      (void *)value,
      (argumentJArrayRef == -1 ? (void *)nullptr : (void *)argumentJArrayRef));
  if (argumentJArrayRef != -1) {
    j_handles_destroy(SaxonProcessor::sxn_environ->thread,
                      (void *)argumentJArrayRef);
    argumentJArrayRef = -1;
  }
  if (results == -2) {
    throw SaxonApiException();
  }
  return getXdmValueSubClass(results);
}
