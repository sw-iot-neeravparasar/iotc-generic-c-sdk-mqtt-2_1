/* Copyright (C) 2020 Avnet - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */

#ifndef IOTCONNECT_EVENT_H
#define IOTCONNECT_EVENT_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        /*
        UNKNOWN_EVENT = 0,
        DEVICE_COMMAND = 0x01,
        DEVICE_OTA = 0x02,
        MODULE_UPDATE_COMMAND = 0x03,
        ON_CHANGE_ATTRIBUTE = 0x10,
        ON_CHANGE_SETTING = 0x11,
        ON_FORCE_SYNC = 0x12,
        ON_ADD_REMOVE_DEVICE = 0x13,
        ON_ADD_REMOVE_RULE = 0x15,
        ON_CLOSE = 0x99
        *///Re-edit

        UNKNOWN_EVENT = 0x10,
        DEVICE_COMMAND = 0,
        DEVICE_OTA = 1,
        MODULE_UPDATE_COMMAND = 2,
        ON_CHANGE_ATTRIBUTE = 101,
        REFRESH_SETTING_TWIN = 102,
        RULE_CHANGE_COMMAND = 103,
        REFRESH_CHILD_DEVICE = 104,
        DF_CHANGE = 105,
        //START_HEARTBEAT = 110,
        STOP_HEARTBEAT = 111,
        DEVICE_DELETE = 106,
        DEVICE_DISABLE = 107,
        Device_connection_status =116,
        STOP_OPERATION = 109,
        HEARTBEAT_COMMAND = 110,
        ON_FORCE_SYNC = 0x12,
        ON_ADD_REMOVE_DEVICE = 0x13,
        ON_ADD_REMOVE_RULE = 0x15,

        //ON_CLOSE = 0x99
        ON_CLOSE = 108

    } IotConnectEventType;


    typedef struct IotclEventDataTag* IotclEventData;

    typedef void (*IotclMessageCallback)(IotclEventData data, IotConnectEventType type);

    typedef void (*IotclRespReceCallback)(IotclEventData root, IotConnectEventType type);

    typedef void (*IotclgetattCallback)(IotclEventData command); //

    typedef void (*IotclgetchCallback)(IotclEventData command);

    typedef void (*IotclRuleChCallback)(IotclEventData command);

    typedef void (*IotclgettwinnCallback)(IotclEventData command);

    typedef void (*IotcltwinrecivCallback)(IotclEventData payload);




    typedef void (*IotclTwinUpCallback)(IotclEventData key, IotclEventData value);

    typedef void (*IotclOtaCallback)(IotclEventData data);

    typedef void (*IotclModCallback)(IotclEventData data);


    typedef void (*IotclGetDfCallback)(IotclEventData data);//

    typedef void (*IotclStartHBCallback)(IotclEventData data);

    typedef void (*IotclStartHBStopCallback)(IotclEventData data);

    typedef void (*IotclCommandCallback)(IotclEventData data);//

    typedef void (*IotclChildDevCallback)(IotclEventData data);

    //callback configuration for the events module
    typedef struct {
        IotclOtaCallback ota_cb; // callback for OTA events.
        IotclModCallback mod_cb;
        IotclCommandCallback cmd_cb; // callback for command events.
        IotclChildDevCallback child_dev;
        IotclMessageCallback msg_cb; // callback for ALL messages, including the specific ones like cmd or ota callback.
        IotclGetDfCallback get_df;
        IotclStartHBCallback hb_cmd;
        IotclStartHBStopCallback hb_stop;
        IotcltwinrecivCallback twin_msg_rciv;
        IotclgetattCallback getatt_cb;
        IotclgetchCallback get_ch;
        IotclRuleChCallback rule_cb;


        IotclgettwinnCallback gettwin_cb;

        IotclRespReceCallback resp_recive;

        IotclTwinUpCallback twin_up_cb;//twin update callback
        IotclCommandCallback unsupported_cb;   // callback when event that cannot be decoded by the library is received.
    } IotclEventFunctions;


    // The user should supply the event received json form the cloud.
    // The function will process the received message and will invoke callbacks accordingly.
    // If return value is false, there was an error during processing.
    bool iotcl_process_event(const char* event);



    bool twin_event(const char* event);

    // Returns a malloc-ed copy of the command line message parameter.
    // The user must manually free the returned string when it is no longer needed.
    char* iotcl_clone_command(IotclEventData data);

    // Returns a malloc-ed copy of the OTA download URL with a given zero-based index.
    // The user must manually free the returned string when it is no longer needed.
    char* iotcl_clone_download_url(IotclEventData data, size_t index);

    // Returns a malloc-ed copy of the OTA firmware version.
    // The user must manually free the returned string when it is no longer needed.
    char* iotcl_clone_sw_version(IotclEventData data);

    // Returns a malloc-ed copy of the OTA hardware version. It only includes the version number, and not the name.
    // The user must manually free the returned string when it is no longer needed.
    char* iotcl_clone_hw_version(IotclEventData data);

    // Returns a malloc-ed copy of the Ack ID of the OTA message or a command.
    // The user must manually free the returned string when it is no longer needed.
    char* iotcl_clone_ack_id(IotclEventData data);

    // Creates an OTA or a command ack json with optional message (can be NULL).
    // The user is responsible to free the returned string.
    // This function also frees up all resources taken by the message.
    char* iotcl_create_ack_string_and_destroy_event(
        IotclEventData data,
        bool success,
        const char* message
    );

    char* create_twin_json(
        const char* key,
        const char* value
    );

    char* prosess_cmd(int command); //edit get attribute

    char* prosess_hb();

    int df_update(IotclEventData root);

    int hb_update(IotclEventData root);

    int hb_event(IotclEventData root);

    char* id_tg(IotclEventData data);

    char* response_string(IotclEventData root, IotConnectEventType type);//responce data



     /*
      Creates an OTA ack json with optional message (can be NULL).
      The user is responsible to free the returned string.
      This function should not be generally used to send the ACK in response to an immediate event.
        use iotcl_create_ack_string_and_destroy_event instead.
      This function is intended to be used for the following firmware update use case:
      o Device an OTA and decides that it needs to update software
      o Device does not send an ack. Device should call iotcl_destroy_event
      o Device records the ack ID and desired firmware version and stores them.
      o Device downloads the firmware and restarts.
      o If there is a failure during bring up, the device falls back to the original firmware.
      o Device or service starts up and compares the stored desired firmware version against the actual running version
      o Device sends the ack according to the the above comparison using the stored ack ID.

     */
    char* iotcl_create_ota_ack_response(
        const char* ota_ack_id,
        bool success,
        const char* message
    );

    // Call this is no ack is sent.
    // This function frees up all resources taken by the message.
    void iotcl_destroy_event(IotclEventData data);

#ifdef __cplusplus
}
#endif

#endif //IOTCONNECT_EVENT_H
