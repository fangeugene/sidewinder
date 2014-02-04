var http = require('http');
var WebSocketServer = require('ws').Server;
var Serialport = require('serialport');
var Uart = Serialport.SerialPort;
var express = require('express')
var async = require('async');
var net = require('net');

var connectedClients = {};
var curClientID = 0;

// Create uart and return it in the callback
function makeUart(callback) {
  var uart = new Uart('/dev/ttyO4', {
    baudrate: 115200,
    parser: Serialport.parsers.readline("\n")
  });
  uart.once("open", function () {
    console.log('UART Open');
    callback(null, uart)
  });
}

// Create main server
function runServer(uart) {
  var app = express();
  app.use(express.static(__dirname + '/public'));
  var server = http.createServer(app);
  server.listen(8080);

  // Websocket setup
  var wss = new WebSocketServer({port: 8081});
  wss.on('connection', function(ws) {
    ws.clientID = curClientID;
    connectedClients[ws.clientID] = ws;
    curClientID += 1;

    ws.on('close', function() {
      delete connectedClients[ws.clientID];
      console.log('Client disconnected!');
    });

    ws.on('message', function(message) {
      console.log('Received input: %s', message);
      uart.write(message + '\n');
    });
  });

  // Testing
  /*
  uart.write('0 180 100 100\n');
  uart.write('0 360 001 092\n');
  uart.write('This is a very very very very very long sentence!\n');
  uart.write('Hello...');
  uart.write('...bye.\n');
  */

  uartHandler(uart);
}

function uartHandler(uart) {
  uart.on('data', function (data) {
    broadcastMsg(data);
  });
}

function broadcastMsg(data) {
  for (var cid in connectedClients) {
    ws = connectedClients[cid];
    try {
      ws.send(data);
    } catch (e) {
      // handle error
    }
  }
}

// Make the uart
async.parallel({
    uart: makeUart
  },
  function(err, r) {
    /*if (err) {
      log.error(err);
      vision.kill();
      process.exit();
    }*/
    runServer(r.uart);
});
