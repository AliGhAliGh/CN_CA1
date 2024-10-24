const express = require('express');
const http = require('http');
const { Server } = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

// Store connected peers with their peerId
let peerIds = {};
let peerNames = {};

io.on('connection', (socket) => {
    console.log(`User connected: ${socket.id}`);

    // When a peer connects, save their peerId
    socket.on('register', (peerName) => {
        // Check if the peerId is provided
        if (!peerName || peerName.trim() === '') {
            // If peerId is not provided, send an error back to the client
            console.log(`Registration failed for ${socket.id}: No peerId provided`);
            return;
        }

        // Store the peerId and map it to the socket ID
        peerIds[peerName] = socket.id;
        peerNames[socket.id] = peerName;
        console.log(`Peer registered: ${peerName} -> ${socket.id}`);
    });

    socket.on('offer', (peerId, sdpOffer) => {
        const targetSocketId = peerIds[peerId];
        const fromPeerName = peerNames[socket.id];

        if (targetSocketId) {
            console.log(`Sending offer from ${fromPeerName} to peer: ${peerId}`);
            io.to(targetSocketId).emit('offer', { sdpOffer, from: fromPeerName });
        }
    });

    socket.on('answer', (peerId, sdpAnswer) => {
        const targetSocketId = peerIds[peerId];
        const fromPeerName = peerNames[socket.id];

        if (targetSocketId) {
            console.log(`Sending answer from ${fromPeerName} to peer: ${peerId}`);
            io.to(targetSocketId).emit('answer', { sdpAnswer, from: fromPeerName });
        }
    });

    socket.on('reject', peerId => {
        const targetSocketId = peerIds[peerId];
        const fromPeerName = peerNames[socket.id];

        if (targetSocketId) {
            console.log(`Sending reject from ${fromPeerName} to peer: ${peerId}`);
            io.to(targetSocketId).emit('reject', { from: fromPeerName });
        }
    });

    socket.on('ice-candidate', (peerId, candidate) => {
        const targetSocketId = peerIds[peerId];
        const fromPeerName = peerNames[socket.id];

        if (targetSocketId) {
            console.log(`Sending ICE candidate from ${fromPeerName} to peer: ${peerId}`);
            io.to(targetSocketId).emit('ice-candidate', { candidate, from: fromPeerName });
        }
    });

    socket.on('disconnect', () => {
        console.log(`User disconnected: ${socket.id}`);

        delete peerNames[socket.id];
        for (const peerId in peerIds) {
            if (peerIds[peerId] === socket.id) {
                delete peerIds[peerId];
                console.log(`Removed peer: ${peerId}`);
                break;
            }
        }
    });
});

// Start the server
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
    console.log(`Signaling server running on port ${PORT}`);
});
