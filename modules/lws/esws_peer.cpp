#ifdef JAVASCRIPT_ENABLED

#include "esws_peer.h"
#include "core/io/ip.h"

void ESWSPeer::set_sock(int p_sock) {

	peer_sock = p_sock;
	in_buffer.clear();
	queue_count = 0;
}

void ESWSPeer::set_write_mode(WriteMode p_mode) {
	write_mode = p_mode;
}

ESWSPeer::WriteMode ESWSPeer::get_write_mode() const {
	return write_mode;
}

void ESWSPeer::read_msg(uint8_t *p_data, uint32_t p_size) {
	WARN_PRINTS(String("Writing: ") + itos(p_size));
	in_buffer.write((uint8_t *)&p_size, 4);
	in_buffer.write(p_data, p_size);
	queue_count++;
}

Error ESWSPeer::put_packet(const uint8_t *p_buffer, int p_buffer_size) {

	int is_bin = write_mode == WebSocketPeer::WRITE_MODE_BINARY ? 1 : 0;
	EM_ASM({
		var sock = Module.IDHandler.get($0);
		var bytes_array = new Uint8Array($2);
		var i = 0;

		for(i=0; i<$2; i++) {
			bytes_array[i] = Module.getValue($1+i, 'i8');
		}

		if ($3) {
			sock.send(bytes_array.buffer);
		} else {
			var string = new TextDecoder("utf-8").decode(bytes_array);
			sock.send(string);
		}
	}, peer_sock, p_buffer, p_buffer_size, is_bin);

	return OK;
};

Error ESWSPeer::get_packet(const uint8_t **r_buffer, int &r_buffer_size) const {

	if (queue_count == 0)
		return ERR_UNAVAILABLE;

	uint32_t to_read = 0;
	uint32_t left = 0;
	r_buffer_size = 0;

	in_buffer.read((uint8_t *)&to_read, 4);
	WARN_PRINTS(String("Reading: ") + itos(to_read));
	--queue_count;
	left = in_buffer.data_left();

	if(left < to_read) {
		in_buffer.advance_read(left);
		return FAILED;
	}

	in_buffer.read(packet_buffer, to_read);
	*r_buffer = packet_buffer;
	r_buffer_size = to_read;

	return OK;
};

int ESWSPeer::get_available_packet_count() const {

	return queue_count;
};

bool ESWSPeer::is_binary_frame() const {

	ERR_FAIL_COND_V(!is_connected_to_host(), false);

	return false;
};

bool ESWSPeer::is_connected_to_host() const {

	return peer_sock != -1;
};

void ESWSPeer::close() {

	if (peer_sock != -1) {
		EM_ASM({
			var sock = Module.IDHandler.get($0);
			sock.close();
			Module.IDHandler.remove($0);
		}, peer_sock);
	}
	peer_sock = -1;
	queue_count = 0;
	in_buffer.clear();
};

IP_Address ESWSPeer::get_connected_host() const {

	return IP_Address();
};

uint16_t ESWSPeer::get_connected_port() const {

	return 1025;
};

ESWSPeer::ESWSPeer() {
	peer_sock = -1;
	queue_count = 0;
	in_buffer.resize(16);
	write_mode = WRITE_MODE_BINARY;
};

ESWSPeer::~ESWSPeer() {

	in_buffer.resize(0);
	close();
};

#endif // JAVASCRIPT_ENABLED
