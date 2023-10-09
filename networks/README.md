# Report

Q1) How is our implementation of data sequencing and retransmission different from traditional TCP?

There are some difference in the implementation:

- We do not perform the 3 way handshake in the beginning. We directly start sending data. Traditional TCP performs the 3 way handshake before sending data.
- We try to send an acknowledgment after every packet. Traditional TCP has the ability to send the acknowledgement of multiple packets together.
- In case the speed of transmission and reception is not the same, we do not perform congestion control. Traditional TCP performs congestion control or flow control.
- We do not use any Synchronisation bit, whereas traditional TCP uses a Synchronisation bit.
- We do not perform the 4 way handshake at the end. Traditional TCP performs the 4 way handshake at the end.
- We also notice there are others flags as well, which we do not implement.

Q2) How can we extend our implementation to account for flow control? We may ignore deadlocks.

Flow control within TCP refers to the situation that arises when there is a disparity between the speed at which data is transmitted and the speed at which it is received. This discrepancy can disrupt the smooth flow of data. To mitigate such issues, conventional TCP employs a window size for both the sender and receiver. This window size represents the maximum amount of data that can be sent or received without encountering any disruptions or problems.

We can have a sliding window kind of approach for the implementation:

- Initially the sender starts slow by sending only one packet. The receiver receives the packet and sends an acknowledgement along with the a certain window size showing that it can receive more packets.
- The sender now sends more number of packets than before, but less than the window size. The receiver keeps getting the packets and as it sends back the acknowledgement it shifts the sliding window too to get more packets. It always sends the window size back so that the sender to accordingly send data packets.
- Once the sender starts sending the same number of packets as the window size, this can continue till the end of the data or if the receiver changes window size, in which case sender will have to adjust accordingly.
- This ensures dynamic updates to window size.