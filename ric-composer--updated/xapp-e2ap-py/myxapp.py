import src.e2ap_xapp as e2ap_xapp
from time import sleep
from ricxappframe.e2ap.asn1 import IndicationMsg

import sys
sys.path.append("oai-oran-protolib/builds/")
from ran_messages_pb2 import *


BER_THRESHOLD = 0.01
MCS_OFFSET_VALUE = -2.0


def xappLogic():

    # instantiate xApp
    connector = e2ap_xapp.e2apXapp()

    # get gNBs connected to RIC
    gnb_id_list = connector.get_gnb_id_list()
    print("{} gNB connected to RIC, listing:".format(len(gnb_id_list)))

    for gnb_id in gnb_id_list:
        print(gnb_id)

    print("---------")

    # subscription requests
    for gnb in gnb_id_list:
        e2sm_buffer = e2sm_report_request_buffer()
        connector.send_e2ap_sub_request(e2sm_buffer, gnb)

    # read loop
    sleep_time = 4

    while True:
        print("Sleeping {}s...".format(sleep_time))
        sleep(sleep_time)

        messgs = connector.get_queued_rx_message()

        if len(messgs) == 0:
            print("{} messages received while waiting".format(len(messgs)))
            print("____")

        else:
            print("{} messages received while waiting, printing:".format(len(messgs)))

            for msg in messgs:

                if msg["message type"] == connector.RIC_IND_RMR_ID:
                    print("RIC Indication received from gNB {}, decoding E2SM payload".format(msg["meid"]))

                    indm = IndicationMsg()
                    indm.decode(msg["payload"])

                    resp = RAN_indication_response()
                    resp.ParseFromString(indm.indication_message)

                    print("Decoded RAN indication response:")
                    print(resp)

                    gnb_id = msg["meid"].decode("ascii") if isinstance(msg["meid"], bytes) else msg["meid"]
                    
                    handle_indication_response(resp, connector, gnb_id)

                    print("___")

                else:
                    print("Unrecognized E2AP message received from gNB {}".format(msg["meid"]))


def handle_indication_response(resp, connector, gnb_id):
    """
    Project 2 logic:
    Read UE BER from indication response.
    If BER is above threshold, send a control request to the gNB.
    """

    for entry in resp.param_map:

        if entry.key == RAN_parameter.UE_LIST:

            ue_list = entry.ue_list

            print("Connected UEs reported by gNB: {}".format(ue_list.connected_ues))

            for ue in ue_list.ue_info:

                print("UE rnti={}, BER={}".format(
                    ue.rnti,
                    ue.ber if ue.HasField("ber") else "N/A",
                ))

                if ue.HasField("ber") and ue.ber > BER_THRESHOLD:
                    print("BER above threshold for UE rnti={}. Sending control request...".format(ue.rnti))

                    control_buffer = e2sm_control_request_buffer(
                        rnti=ue.rnti,
                        apply_control=True,
                        mcs_offset=MCS_OFFSET_VALUE
                    )

                    connector.send_e2ap_control_request(control_buffer, gnb_id)

                    print("Control request sent for UE rnti={}, apply_control=True, mcs_offset={}".format(
                        ue.rnti,
                        MCS_OFFSET_VALUE
                    ))

                else:
                    print("BER below threshold for UE rnti={}. No control needed.".format(ue.rnti))


def e2sm_report_request_buffer():
    master_mess = RAN_message()
    master_mess.msg_type = RAN_message_type.INDICATION_REQUEST

    inner_mess = RAN_indication_request()
    inner_mess.target_params.extend([
        RAN_parameter.GNB_ID,
        RAN_parameter.UE_LIST
    ])

    master_mess.ran_indication_request.CopyFrom(inner_mess)

    buf = master_mess.SerializeToString()
    return buf


def e2sm_control_request_buffer(rnti, apply_control, mcs_offset):
    """
    Build a CONTROL_REQUEST message.
    The xApp sends per-UE control properties to the gNB.
    """

    master_mess = RAN_message()
    master_mess.msg_type = RAN_message_type.CONTROL

    control_mess = RAN_control_request()

    # Create one map entry for UE_LIST
    entry = control_mess.target_param_map.add()
    entry.key = RAN_parameter.UE_LIST

    # Fill UE list
    entry.ue_list.connected_ues = 1

    ue = entry.ue_list.ue_info.add()
    ue.rnti = rnti
    ue.apply_control = apply_control
    ue.mcs_offset = mcs_offset

    master_mess.ran_control_request.CopyFrom(control_mess)

    return master_mess.SerializeToString()


if __name__ == "__main__":
    xappLogic()