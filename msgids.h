///////////////////////////////////////////////////////////////////////////////
/// MSGIDS.H
///
/// This include file contains macros to map the numeric message IDs to
/// sensible names.
///
/// These should be consecutive up to a maximum of MSG_MAX_MSG_IDS (currently
/// 26, defined in 'mq.h' in the kernel).
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _MSGIDS_H_
#define _MSGIDS_H_

#define MSG_ID_UPDATE_7SEG	1
#define MSG_ID_KEY_PRESSED  2
#define MSG_ID_KEY_RELEASED	3

#define MSG_ID_INIT_COMPLETE	4
#define MSG_ID_NEW_ACTUAL_RPS	5
#define MSG_ID_NEW_DEMAND_RPS	6
#define MSG_ID_NEW_RPS_ENTERED	7

#define MSG_ID_DATALOG_LOGEVENT		8
#define MSG_ID_DATALOG_DELETELOG	9
#define MSG_ID_DATALOG_DUMPLOG		10

#endif
