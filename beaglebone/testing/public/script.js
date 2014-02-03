var ws = new MozWebSocket('ws://' + window.location.hostname + ':8081');
var enable = false;

ws.onopen = function() {
  // Web Socket is connected, send data using send()
  ws.send("Browser says HIHIHI!");
  console.log("Message is sent...");
};
ws.onmessage = function (evt) {
  var received_msg = evt.data;
  console.log(received_msg);
  var split_msg = received_msg.split(' ');
  if (split_msg[0] == '0') {
    $('#robot').attr('transform', 'rotate(' + -parseInt(split_msg[1]) + ', 100, 100)');
  } else if (split_msg[0] == '1') {
    $('#module0').attr('transform', 'rotate(' + -parseInt(split_msg[2]) + ', 100, 25)');
    $('#module1').attr('transform', 'rotate(' + -parseInt(split_msg[4]) + ', 35, 137.5)');
    $('#module2').attr('transform', 'rotate(' + -parseInt(split_msg[6]) + ', 165, 137.5)');

    $('#module0power').attr('r', Math.log(parseInt(split_msg[1])));
    $('#module1power').attr('r', Math.log(parseInt(split_msg[3])));
    $('#module2power').attr('r', Math.log(parseInt(split_msg[5])));
  } else if (split_msg[0] == '9') {
    if (split_msg[1] == '1') {
      $('#enable-status').css('background-color', 'green');
      $('#enable-button').text('Disable!');
      enable = true;
    } else {
      $('#enable-status').css('background-color', 'red');
      $('#enable-button').text('Enable!');
      enable = false;
    }
  }
};
ws.onclose = function() {
  // websocket is closed.
  console.log("Connection is closed...");
};

$(document).ready(function(){
  setInterval(function() { ws.send('9'); }, 20);  // watchdog

  $('#enable-button').click(function() {
    if (enable) {
      ws.send('0 0');
    } else {
      ws.send('0 1');
    }
  });

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
    } else {
      send_movement(controller.axes[1], controller.axes[0], controller.axes[3]);
    }
  }, 10);
});

function pad(num, size) {
    var s = num+"";
    while (s.length < size) s = "0" + s;
    return s;
}
