var net = require('net');

var server = net.createServer();

let sockets = [];
server.on('connection', function (sock) {
    console.log('CONNECTED: ' + sock.remoteAddress + ':' + sock.remotePort);
    sockets.push(sock);

    fulldata = ''
    sock.on('data', function (data) {
        //        console.log('DATA ' + sock.remoteAddress + ': ' + data);
        let cl = data.indexOf('Content-Length')
        if ((data.indexOf('PUT') == 0 || data.indexOf('POST') == 0 || data.indexOf('GET') == 0) && (cl > 0)) {
            fulldata = data
        } else {
            fulldata += data
            console.log(fulldata);
            let date_ob = (new Date()).toUTCString()
            let res =
`HTTP/1.1 200 OK\r
Vary: Accept-Encoding\r
X-Frame-Options: SAMEORIGIN\r
Content-Type: application/json\r
X-Content-Type-Options: nosniff\r
Content-Length: 0\r
Date: ${date_ob}\r
X-XSS-Protection: 1; mode=block\r
Connection: Keep-Alive\r
Accept-Ranges: bytes\r
\r
\r
`
    sock.write(res)

        }



        // Write the data back to all the connected, the client will receive it as data from the server
    });
});

server.listen(80);