void create_udp_send_handle(int index)
{
	int port = s_send_room_port[index];
	frame_info_t frame_info;
	stream_send_cond_t   stream_info;
	int ipaddr, gateway, netmask;
	
	udp_send_module_handle   *udp_send_hand = NULL;
	send_udp_handle   *udp_hand = NULL;
	
	int ret = sys_get_network(ENC_INPUT_1, &ipaddr, &gateway, &netmask);
	 memset(&stream_info, 0, sizeof(stream_send_cond_t));
	stream_info.video_port = port;
//	sprintf(stream_info.ip, "%s", sroom_IP[index]);
	sprintf(stream_info.ip, "%s", ROOM_IP);
	PRINTF("index=%d,port=0x%x,%s\n",index,port,stream_info.ip);
	
	udp_send_hand = UdpSend_init(&stream_info);
	if(NULL == udp_send_hand){
		ERR_PRN("UdpSend_init failed!index=%d\n",index);
	}
	s_pSend_handle[index]=udp_send_hand;
	PRINTF("s_pSend_handle[%d]=%p\n",index,s_pSend_handle[index]);
}
