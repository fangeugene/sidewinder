var ws = new MozWebSocket('ws://' + window.location.hostname + ':8081');
var enable = false;

ws.onopen = function() {
  // Web Socket is connected, send data using send()
  ws.send("Browser says HIHIHI!");
  console.log("Message is sent...");
};
ws.onmessage = function (evt) {
  var received_msg = evt.data;
  var split_msg = received_msg.split(' ');
  if (split_msg[0] == '0') {
    $('#robot').attr('transform', 'rotate(' + -parseInt(split_msg[1]) + ', 100, 100)');
  } else if (split_msg[0] == '1') {
    $('#module0').attr('transform', 'rotate(' + -parseInt(split_msg[2]) + ', 100, 25)');
    $('#module1').attr('transform', 'rotate(' + -parseInt(split_msg[4]) + ', 35, 137.5)');
    $('#module2').attr('transform', 'rotate(' + -parseInt(split_msg[6]) + ', 165, 137.5)');

    var m0power = parseInt(split_msg[1]);
    $('#module0power').attr('r', Math.log(Math.abs(m0power)));
    if (m0power >= 0) {
      $('#module0power').attr('cy', '15');
    } else {
      $('#module0power').attr('cy', '35');
    }
    var m1power = parseInt(split_msg[3]);
    $('#module1power').attr('r', Math.log(Math.abs(m1power)));
    if (m1power >= 0) {
      $('#module1power').attr('cy', '127.5');
    } else {
      $('#module1power').attr('cy', '147.5');
    }
    var m2power = parseInt(split_msg[5]);
    $('#module2power').attr('r', Math.log(Math.abs(m2power)));
    if (m2power >= 0) {
      $('#module2power').attr('cy', '127.5');
    } else {
      $('#module2power').attr('cy', '147.5');
    }

  } else if (split_msg[0] == '9') {
    if (split_msg[1] == '1') {
      $('#enable-status').css('background-color', 'green');
      enable = true;
    } else {
      $('#enable-status').css('background-color', 'red');
      enable = false;
    }
  }
};
ws.onclose = function() {
  // websocket is closed.
  console.log("Connection is closed...");
};

$(document).ready(function(){
  setInterval(function() { ws.send('9'); }, 50);  // watchdog

  var last_enable_msg = '';
  function send_enable(e) {
    var msg = '0 ' + e;
    if (last_enable_msg != msg) {
      ws.send(msg);
    }
    last_enable_msg = msg;
  }

  var theta = 0;
  var last_movement_msg = '';
  function send_movement(x, y, w) {
    x *= -100;
    y *= -100;
    w *= -100;

    if (w < 25 && w > -25) {
      w = 0;
    }

    var r = Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2));
    if (Math.round(x) == 0 && Math.round(y) == 0) {
      r = 1;
    } else {
      theta = Math.atan(y/x);
      if (x <= 0) {
        theta += Math.PI;
      }
      if (theta > Math.PI) {
        theta -= 2 * Math.PI;
      }
      theta = theta * 180 / Math.PI;
    }

    var msg = '1 ' + pad(parseInt(r), 3) +  ' ' + pad(parseInt(theta+500), 3) + ' ' + pad(parseInt(w+500), 3);

    if (last_movement_msg != msg) {
      ws.send(msg);
      last_movement_msg = msg;
    }
  }

  // Joystick
  var controller = null;
  function connecthandler(e) {
    $("#joystick-status").text("Joystick Connected!");
    controller = e.gamepad;
  }

  function disconnecthandler(e) {
    $("#joystick-status").text("Joystick Not Connected");
    controller = null;
  }
  window.addEventListener("MozGamepadConnected", connecthandler);
  window.addEventListener("MozGamepadDisconnected", disconnecthandler);

  setInterval(function() {
    if (controller == null) {
      send_movement(0, 0, 0);
      send_enable(0);
    } else {
      send_movement(controller.axes[1], controller.axes[0], controller.axes[3]);
      send_enable(controller.buttons[5]);
    }
  }, 20);
});

function pad(num, size) {
    var s = num+"";
    while (s.length < size) s = "0" + s;
    return s;
}
