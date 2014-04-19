var http = require('http');
var WebSocketServer = require('ws').Server;
var Serialport = require('serialport');
var Uart = Serialport.SerialPort;
var express = require('express')
var async = require('async');
var net = require('net');
var exec = require('child_process').exec;
var spawn = require('child_process').spawn;

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

// Create the socket for the OpenCV IPC, and return it in the callback
function makeSocket(callback) {
  var socket = net.createServer(function(connection) { //'connection' listener
    console.log('webcam connected');
    connection.on('end', function() {
      console.log('webcam disconnected');
    });
    callback(null, connection);
  });
  socket.listen('/tmp/opencv.sock', function() { //'listening' listener
    console.log('server bound');
  });
  process.on( 'SIGINT', function() { socket.close(); });
  process.on( 'SIGTERM', function() { socket.close(); });
  process.on( 'uncaughtException', function() { socket.close(); });
}

// Create main server
function runServer(uart, socket) {
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

  uartHandler(uart);
  socketHandler(socket);
}

function socketHandler(socket) {
  socket.on('data', function(data) {
    console.log(data.toString());
  });
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
    uart: makeUart,
    socket: makeSocket
  },
  function(err, r) {
    /*if (err) {
      log.error(err);
      vision.kill();
      process.exit();
    }*/
    runServer(r.uart, r.socket);
});

// For global access
var vision;
function monitorProcess(spawned) {
  vision = spawned;
  spawned.stdout.on('data', function (data) {
    console.log('stdout: ' + data);
  });
  spawned.stderr.on('data', function (data) {
    // console.log('stderr: ' + data);
  });
  spawned.on('close', function (code) {
    console.log('child process exited with code ' + code);
    // console.log('attemting to restart child');
    // monitorProcess(spawn(cmd, args));
  });
}
var cmd = '../mjpg_streamer/mjpg_streamer';
var args = ['-i','../mjpg_streamer/input_uvc.so -r 320x240 -f 30',
            '-o','../mjpg_streamer/output_http.so -p 8081',
            '-o','../mjpg_streamer/output_opencv.so']
monitorProcess(spawn(cmd, args));

// Cleanup code for when killed with C-c
process.on( 'SIGINT', function() {
  console.log("Kill signal recieved");
  vision.kill();
  process.exit();
});

// Cleanup code for when killed by system
process.on( 'SIGTERM', function() {
  console.log("Kill signal recieved");
  vision.kill();
  process.exit();
});

// Cleanup code for exceptions
process.on('uncaughtException', function(err) {
  // console.log(err.stack);
  // vision.kill();
  // process.exit();
});
