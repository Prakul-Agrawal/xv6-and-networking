# Report

Q1) How is my implementation of data sequencing and retransmission different from traditional TCP?

There are some difference in the implementation:

- We do not perform the 3 way handshake in the beginning. We directly start sending data. Traditional TCP performs the 3 way handshake before sending data.
- We try to send an acknowledgment after every packet. Traditional TCP has the ability to send the acknowledgement of multiple packets together.
- In case the speed of transmission and reception is not the same, we do not perform congestion control. Traditional TCP performs congestion control or flow control.
- We do not use any Synchronisation bit, whereas traditional TCP uses a Synchronisation bit.
- We do not perform the 4 way handshake at the end. Traditional TCP performs the 4 way handshake at the end.
- We also notice there are others flags as well, which we do not implement.