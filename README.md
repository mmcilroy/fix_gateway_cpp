# fix_gateway_cpp

Prototype of a stateless application designed to route FIX messages between client and venue FIX sessions. The basic idea is to embed data inside the ClOrdID of the message being routed to the venue that can extracted from responses to work out how to route the message back to the client.

**Example**
            | FIX
--------------------------------------
From client | 35=D|11=12345|55=VOD.L
To venue    | 35=D|11=1#12345|55=VOD.L
From venue  | 35=8|11=1#12345|55=VOD.L
To client   | 35=8|11=12345|55=VOD.L

This approach breaks down when no ClOrdID is available - for example business rejects.

The application is designed as an event driven application.

* Event sources are responsible for generating the events to process (in our case an event is FIX message)
* Event sinks publish output events to interested parties (clients and venues)
* Business logic is handled by a single thread. We are using lua to implement the business logic (building and routing of FIX messages), but any language could easily be used
* With this design it should be possible to build a stateful application in the future. All input events can be persisted to disk and recovered on startup to rebuild the applications internal state

Check out scripts/gateway.lua for a simple example
