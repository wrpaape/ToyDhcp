Name:       William Reid Paape


NetID:      paape


Student ID: 2877379718



## Compilation Steps
```
make
```

## Run
```
# on a single host...
# open a terminal and start the server
./Server
# open another terminal and start the client
./Client
```

## Clean
```
make clean
```

## Example Output
```
# in the Server terminal...
./Server

# in the Client terminal
./Client
Client: send_discovery() - sending discovery 200...sent
Client: receive_offer() - receiving offer...received offer 239 (205.97.126.200)
Client: send_request() - sending request 7 (205.97.126.200)...sent
Client: receive_ack() - receiving ACK...received ack 177 (205.97.126.200)

# observed in the Server terminal...
================================================================================
Server: handle_client() - handling client 127.0.0.1
Server: receive_discovery() - receiving discovery...received discovery 200
Server: send_offer() - sending offer 239 (205.97.126.200)...sent
Server: receive_request() - receiving request...received request 7 (205.97.126.200)
Server: send_ack() - sending ACK 177 (205.97.126.200)...sent
================================================================================
```
`205.97.126.200` is the offered/requested IPv4 address, `200` is the
"discovery" transaction ID, `239` is the "offer" transaction ID, `7` is the
"request" transaction ID, and `177` is the "ACK" transaction ID.

## Server Routines
1. `receive_discovery()` - Receives the discovery message from the client and
saves the transaction id for use in constructing the offer.
2. `send_offer()` - Sends the IPv4 address offer to the client.
3. `receive_request()` - Receives the request from the client and saves the embedded IPv4 address for use in constructing the ACK.
4. `send_ack()` - Sends an acknowledgement of the client's requested IP address.

## Client Routines
1. `send_discovery()` - Sends a randomly-generated 8-bit transaction ID to the server.
2. `receive_offer()` - Receives the IPv4 address from the server.
3. `send_request()` - Replies to the offer to claim the offered IP address.
4. `receive_ack()` - Receives the acknowledgement of the request from the server indicating that the requested IP address belongs to the client.
