//
//  BSG_KSCrashReportFields.h
//
//  Created by Karl Stenerud on 2012-10-07.
//
//  Copyright (c) 2012 Karl Stenerud. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef HDR_BSG_KSCrashReportFields_h
#define HDR_BSG_KSCrashReportFields_h

#pragma mark - Report Types -

#define BSG_KSCrashReportType_Minimal "minimal"
#define BSG_KSCrashReportType_Standard "standard"
#define BSG_KSCrashReportType_Custom "custom"

#pragma mark - Memory Types -

#define BSG_KSCrashMemType_Block "objc_block"
#define BSG_KSCrashMemType_Class "objc_class"
#define BSG_KSCrashMemType_NullPointer "null_pointer"
#define BSG_KSCrashMemType_Object "objc_object"
#define BSG_KSCrashMemType_String "string"
#define BSG_KSCrashMemType_Unknown "unknown"

#pragma mark - Exception Types -

#define BSG_KSCrashExcType_CPPException "cpp_exception"
#define BSG_KSCrashExcType_Deadlock "deadlock"
#define BSG_KSCrashExcType_Mach "mach"
#define BSG_KSCrashExcType_NSException "nsexception"
#define BSG_KSCrashExcType_Signal "signal"
#define BSG_KSCrashExcType_User "user"

#pragma mark - Common -

#define BSG_KSCrashField_Address "address"
#define BSG_KSCrashField_Contents "contents"
#define BSG_KSCrashField_Exception "exception"
#define BSG_KSCrashField_FirstObject "first_object"
#define BSG_KSCrashField_Index "index"
#define BSG_KSCrashField_Ivars "ivars"
#define BSG_KSCrashField_Language "language"
#define BSG_KSCrashField_Name "name"
#define BSG_KSCrashField_ReferencedObject "referenced_object"
#define BSG_KSCrashField_Type "type"
#define BSG_KSCrashField_UUID "uuid"
#define BSG_KSCrashField_Value "value"

#define BSG_KSCrashField_Error "error"
#define BSG_KSCrashField_JSONData "json_data"

#pragma mark - Notable Address -

#define BSG_KSCrashField_Class "class"
#define BSG_KSCrashField_LastDeallocObject "last_deallocated_obj"

#pragma mark - Backtrace -

#define BSG_KSCrashField_InstructionAddr "instruction_addr"
#define BSG_KSCrashField_LineOfCode "line_of_code"
#define BSG_KSCrashField_ObjectAddr "object_addr"
#define BSG_KSCrashField_ObjectName "object_name"
#define BSG_KSCrashField_SymbolAddr "symbol_addr"
#define BSG_KSCrashField_SymbolName "symbol_name"

#pragma mark - Stack Dump -

#define BSG_KSCrashField_DumpEnd "dump_end"
#define BSG_KSCrashField_DumpStart "dump_start"
#define BSG_KSCrashField_GrowDirection "grow_direction"
#define BSG_KSCrashField_Overflow "overflow"
#define BSG_KSCrashField_StackPtr "stack_pointer"

#pragma mark - Thread Dump -

#define BSG_KSCrashField_Backtrace "backtrace"
#define BSG_KSCrashField_Basic "basic"
#define BSG_KSCrashField_Crashed "crashed"
#define BSG_KSCrashField_CurrentThread "current_thread"
#define BSG_KSCrashField_DispatchQueue "dispatch_queue"
#define BSG_KSCrashField_NotableAddresses "notable_addresses"
#define BSG_KSCrashField_Registers "registers"
#define BSG_KSCrashField_Skipped "skipped"
#define BSG_KSCrashField_Stack "stack"

#pragma mark - Binary Image -

#define BSG_KSCrashField_CPUSubType "cpu_subtype"
#define BSG_KSCrashField_CPUType "cpu_type"
#define BSG_KSCrashField_ImageAddress "image_addr"
#define BSG_KSCrashField_ImageVmAddress "image_vmaddr"
#define BSG_KSCrashField_ImageSize "image_size"

#pragma mark - Memory -

#define BSG_KSCrashField_Free "free"
#define BSG_KSCrashField_Usable "usable"

#pragma mark - Error -

#define BSG_KSCrashField_Backtrace "backtrace"
#define BSG_KSCrashField_Code "code"
#define BSG_KSCrashField_CodeName "code_name"
#define BSG_KSCrashField_CPPException "cpp_exception"
#define BSG_KSCrashField_ExceptionName "exception_name"
#define BSG_KSCrashField_Mach "mach"
#define BSG_KSCrashField_NSException "nsexception"
#define BSG_KSCrashField_Reason "reason"
#define BSG_KSCrashField_Signal "signal"
#define BSG_KSCrashField_Subcode "subcode"
#define BSG_KSCrashField_UserReported "user_reported"
#define BSG_KSCrashField_Overrides "overrides"
#define BSG_KSCrashField_HandledState "handledState"
#define BSG_KSCrashField_Metadata "metaData"
#define BSG_KSCrashField_State "state"
#define BSG_KSCrashField_Config "config"
#define BSG_KSCrashField_DiscardDepth "depth"

#pragma mark - Process State -

#define BSG_KSCrashField_LastDeallocedNSException "last_dealloced_nsexception"
#define BSG_KSCrashField_ProcessState "process"

#pragma mark - App Stats -

#define BSG_KSCrashField_ActiveTimeSinceCrash "active_time_since_last_crash"
#define BSG_KSCrashField_ActiveTimeSinceLaunch "active_time_since_launch"
#define BSG_KSCrashField_AppActive "application_active"
#define BSG_KSCrashField_AppInFG "application_in_foreground"
#define BSG_KSCrashField_BGTimeSinceCrash "background_time_since_last_crash"
#define BSG_KSCrashField_BGTimeSinceLaunch "background_time_since_launch"
#define BSG_KSCrashField_LaunchesSinceCrash "launches_since_last_crash"
#define BSG_KSCrashField_SessionsSinceCrash "sessions_since_last_crash"
#define BSG_KSCrashField_SessionsSinceLaunch "sessions_since_launch"

#pragma mark - Report -

#define BSG_KSCrashField_Crash "crash"
#define BSG_KSCrashField_Diagnosis "diagnosis"
#define BSG_KSCrashField_ID "id"
#define BSG_KSCrashField_ProcessName "process_name"
#define BSG_KSCrashField_Report "report"
#define BSG_KSCrashField_Timestamp "timestamp"
#define BSG_KSCrashField_Version "version"

#pragma mark Minimal
#define BSG_KSCrashField_CrashedThread "crashed_thread"

#pragma mark Standard
#define BSG_KSCrashField_AppStats "application_stats"
#define BSG_KSCrashField_BinaryImages "binary_images"
#define BSG_KSCrashField_SystemAtCrash "system_atcrash"
#define BSG_KSCrashField_System "system"
#define BSG_KSCrashField_Memory "memory"
#define BSG_KSCrashField_Threads "threads"
#define BSG_KSCrashField_User "user"
#define BSG_KSCrashField_UserAtCrash "user_atcrash"

#pragma mark Incomplete
#define BSG_KSCrashField_Incomplete "incomplete"
#define BSG_KSCrashField_RecrashReport "recrash_report"

#endif
