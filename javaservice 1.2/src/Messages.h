//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: EVENT_GENERIC_INFORMATION
//
// MessageText:
//
//  %1
//
#define EVENT_GENERIC_INFORMATION        0x40000001L

//
// MessageId: EVENT_SERVICE_STARTED
//
// MessageText:
//
//  The %1 service has started.
//
#define EVENT_SERVICE_STARTED            0x40000004L

//
// MessageId: EVENT_SERVICE_STOPPED
//
// MessageText:
//
//  The %1 service has stopped.
//
#define EVENT_SERVICE_STOPPED            0x40000005L

//
// MessageId: EVENT_GENERIC_ERROR
//
// MessageText:
//
//  %1
//
#define EVENT_GENERIC_ERROR              0xC0001000L

//
// MessageId: EVENT_FUNCTION_FAILED
//
// MessageText:
//
//  The %1 function failed for the following reason: %2.
//
#define EVENT_FUNCTION_FAILED            0xC0001001L

//
// MessageId: EVENT_START_FAILED
//
// MessageText:
//
//  The %1 service failed to start.
//
#define EVENT_START_FAILED               0xC0001002L

//
// MessageId: EVENT_STOP_FAILED
//
// MessageText:
//
//  The stop method of the %1 service failed to run, the service is being terminated.
//
#define EVENT_STOP_FAILED                0xC0001003L

//
// MessageId: EVENT_STOP_TIMEDOUT
//
// MessageText:
//
//  The %1 service has timed out during a stop request and is being terminated.
//
#define EVENT_STOP_TIMEDOUT              0xC0001004L

