/*
 * File: abb.js
 * Copyright (c) 2023 Loupe
 * https://loupe.team
 * 
 * This file is part of A3BR, licensed under the MIT License.
 * 
 */

const http = require('http');

const requestListener = function (req, res) {

    console.log(req.httpVersion)
    console.log(req.method)
    console.log(req.url)
    let data = '';
    req.on('data', chunk => {
      data += chunk;
    })
    req.on('end', () => {
      console.log(data) // 'Buy the milk'
    })

    setTimeout(function(){
        res.chunkedEncoding = false
        res.writeHead(200);
        res.end();    
    }, 2)

}

const server = http.createServer(requestListener);
server.listen(80);