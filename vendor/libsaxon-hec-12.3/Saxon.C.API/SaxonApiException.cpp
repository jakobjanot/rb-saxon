// SaxonApiException.cpp : Defines the exported functions for the DLL
// application.
//

#include "SaxonApiException.h"

#ifdef MEM_DEBUG
#define new new (__FILE__, __LINE__)
#endif

const char *SaxonApiException::what() {
  if (message == nullptr && !cppException) {
    allocFn fn = operator new;
    if (isStaticError) {
      message = j_getCombinedStaticErrorMessages(
          SaxonProcessor::sxn_environ->thread, (void *)fn);
    } else {
      message =
          j_getErrorMessage(SaxonProcessor::sxn_environ->thread, (void *)fn);
    }
  }
  return message;
}

SaxonApiException::SaxonApiException(bool staticError) {
  message = nullptr;
  lineNumber = -1;
  errorCode = nullptr;
  systemId = nullptr;
  cppException = false;
  isStaticError = staticError;
}

SaxonApiException::SaxonApiException(const SaxonApiException &ex) {
  message = ex.message;
  lineNumber = ex.lineNumber;
  errorCode = ex.errorCode;
  systemId = ex.systemId;
  cppException = ex.cppException;
  isStaticError = ex.isStaticError;
}

SaxonApiException::SaxonApiException(const char *m) {
  if (m != nullptr) {
    // message = m;
    size_t destination_size = strlen(m);
    message = (char *)malloc(sizeof(char) * destination_size);
    snprintf(message, destination_size, "%s", m);
  } else {
    size_t destination_size = 18;
    message = (char *)malloc(sizeof(char) * destination_size);
    snprintf(message, destination_size, "%s", "unknown exception");
  }

  lineNumber = -1;
  errorCode = nullptr;
  systemId = nullptr;
  cppException = true;
  isStaticError = false;
}

SaxonApiException::SaxonApiException(const char *m, const char *ec,
                                     const char *sysId, int linenumber) {
  if (m != nullptr) {
    // message = (char *)m;
    size_t destination_size = strlen(m);
    message = (char *)malloc(sizeof(char) * destination_size);
    snprintf(message, destination_size, "%s", m);
  } else {
    message = nullptr;
  }

  lineNumber = linenumber;

  if (ec != nullptr) {
    size_t destination_size = strlen(ec);
    errorCode = (char *)malloc(sizeof(char) * destination_size);
    snprintf(errorCode, destination_size, "%s", ec);
    // std::cerr<<"SaxonApiException =======: errorCode =
    // "<<errorCode<<std::endl;
  } else {
    errorCode = nullptr;
    // std::cerr<<"SaxonApiException =======: errorCode is empty string
    // "<<std::endl;
  }

  if (sysId != nullptr) {
    size_t destination_size = strlen(sysId);
    systemId = (char *)malloc(sizeof(char) * destination_size);
    snprintf(systemId, destination_size, "%s", sysId);
  } else {
    systemId = nullptr;
  }
  cppException = true;
  isStaticError = false;
}

SaxonApiException::~SaxonApiException() throw() {
  if (message != nullptr) {
    delete message;
  }
  if (errorCode != nullptr) {
    delete errorCode;
  }
  if (systemId != nullptr) {
    delete systemId;
  }
  if (!cppException) {
    j_clearException(SaxonProcessor::sxn_environ->thread);
  }
}

/**
 * Get the error code associated with the ith exception in the vector, if there
 * is one
 * @param i - ith exception in the vector
 * @return the associated error code, or nullptr if no error code is available
 */
const char *SaxonApiException::getErrorCode() {
  if (errorCode == nullptr && !cppException) {
    allocFn fn = operator new;
    errorCode = j_getErrorCode(SaxonProcessor::sxn_environ->thread, (void *)fn);
  }
  return errorCode;
}

int SaxonApiException::getLineNumber() {
  if (lineNumber == -1 && !cppException) {
    lineNumber = j_getLineNumber(SaxonProcessor::sxn_environ->thread);
  }
  return lineNumber;
}

const char *SaxonApiException::getSystemId() {
  if (systemId == nullptr && !cppException) {
    allocFn fn = operator new;
    systemId = j_getSystemId(SaxonProcessor::sxn_environ->thread, (void *)fn);
  }
  return systemId;
}

const char *SaxonApiException::getMessage() {
  if (message == nullptr && !cppException) {
    allocFn fn = operator new;
    if (isStaticError) {
      message = j_getCombinedStaticErrorMessages(
          SaxonProcessor::sxn_environ->thread, (void *)fn);
    } else {
      message =
          j_getErrorMessage(SaxonProcessor::sxn_environ->thread, (void *)fn);
    }
  }

  return message;
}

const char *SaxonApiException::getMessageWithErrorCode() {

  if (!cppException) {
    allocFn fn = operator new;
    if (isStaticError) {
      return j_getCombinedStaticErrorMessages(
          SaxonProcessor::sxn_environ->thread, (void *)fn);
    } else {
      return j_getErrorMessageWithErrorCode(SaxonProcessor::sxn_environ->thread,
                                            (void *)fn);
    }
  }
  return message;
}

const char *SaxonApiException::getCombinedStaticErrorMessages() {
  allocFn fn = operator new;
  return j_getCombinedStaticErrorMessages(SaxonProcessor::sxn_environ->thread,
                                          (void *)fn);
}

int SaxonApiException::staticErrorCount() {
  return j_staticErrorCount(SaxonProcessor::sxn_environ->thread);
}
