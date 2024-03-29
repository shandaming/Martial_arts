set(DEBUG_MODE 1)

macro(LOG_DEBUG info)
  if(DEBUG_MODE)
    message("[DEBUG] ${info}")
  endif()
endmacro()

macro(LOG_FATAL info)
  message(FATAL_ERROR "${info}")
endmacro()
