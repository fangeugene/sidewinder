var ws;
try {
  ws = new MozWebSocket('ws://' + window.location.hostname + ':8081');
} catch (e) {
  ws = new WebSocket('ws://' + window.location.hostname + ':8081');
}
var enable = false;
var deadman = false;

ws.onopen = function() {
  // Web Socket is connected, send data using send()
  ws.send("Browser says HIHIHI!");
  console.log("Message is sent...");
};
ws.onmessage = function (evt) {
  var received_msg = evt.data;
  var split_msg = received_msg.split(' ');
  switch(split_msg[0]) {
    case '0': {
      $('#robot').attr('transform', 'rotate(' + -parseInt(split_msg[1]) + ', 100, 100)');
      break;
    }
    case '1': {
      $('#module0').attr('transform', 'rotate(' + -parseInt(split_msg[2]) + ', 100, 25)');
      $('#module1').attr('transform', 'rotate(' + -parseInt(split_msg[4]) + ', 35, 137.5)');
      $('#module2').attr('transform', 'rotate(' + -parseInt(split_msg[6]) + ', 165, 137.5)');

      var m0power = parseInt(split_msg[1]);
      var m0r = 0;
      if (m0power != 0) {
        m0r = Math.log(Math.abs(m0power));
      }
      $('#module0power').attr('r', m0r);
      if (m0power >= 0) {
        $('#module0power').attr('cy', '15');
      } else {
        $('#module0power').attr('cy', '35');
      }

      var m1power = parseInt(split_msg[3]);
      var m1r = 0;
      if (m1power != 0) {
        m1r = Math.log(Math.abs(m1power));
      }
      $('#module1power').attr('r', m1r);
      if (m1power >= 0) {
        $('#module1power').attr('cy', '127.5');
      } else {
        $('#module1power').attr('cy', '147.5');
      }

      var m2power = parseInt(split_msg[5]);
      var m2r = 0;
      if (m2power != 0) {
        m2r = Math.log(Math.abs(m2power));
      }
      $('#module2power').attr('r', m2r);
      if (m2power >= 0) {
        $('#module2power').attr('cy', '127.5');
      } else {
        $('#module2power').attr('cy', '147.5');
      }
      break;
    }
    case '9': {
      if (split_msg[1] == '1') {
        if (!deadman) {
          $("#robot-status").text("Deadman Pressed");
          $("#robot-status").removeClass("status-negative");
          $("#robot-status").addClass("status-positive");
          deadman = true;
        }
      } else {
        if (deadman) {
          $("#robot-status").text("Deadman Released");
          $("#robot-status").removeClass("status-positive");
          $("#robot-status").addClass("status-negative");
          deadman = false;
        }
      }
      break;
    }
    case 'c': {
      var x = parseInt(split_msg[1]);
      var y = parseInt(split_msg[2]);
      var target_size = 5;
      $('#target').css('width', target_size + 'px');
      $('#target').css('height', target_size + 'px');

      if (x == -1 || y == -1) {
        $('#target').css('visibility', 'hidden');
      } else {
        $('#target').css('visibility', 'visible');
        var width = $('#camera-view').width();
        var scale = width / 320;
        $('#target').css('top', y * scale - target_size / 2);
        $('#target').css('left', x * scale - target_size / 2);
      }
      break;
    }
  }
};
ws.onclose = function() {
  // websocket is closed.
  console.log("Connection is closed...");
};

$(document).ready(function(){
  setInterval(function() { ws.send('9'); }, 50);  // watchdog

  $('#tab-teleop-wc').click(function() {
    ws.send('8 0');
  });

  $('#tab-teleop-rc').click(function() {
    ws.send('8 2');
  });

  $('#tab-tracking').click(function () {
    ws.send('8 1');
  });

  // $('#disable-btn').click(function () {
  //   var btn = $(this);
  //   btn.button('toggle');
  //   enable = !enable;
  //   if (enable) {
  //     btn.text('Enabled');
  //   } else {
  //     btn.text('Disabled');
  //   }
  // });

  // Spacebar disable
  $(window).keypress(function(e) {
    if (e.keyCode == 0) {
      if (enable) {
        enable = false;
        $('#disable-btn').text('Disabled');
        $('#disable-btn').button('toggle');
      }
    } else if (e.keyCode = 13) {
      if (!enable) {
        enable = true;
        $('#disable-btn').text('Enabled');
        $('#disable-btn').button('toggle');
      }
    }
  });

  function reset_gyro(r) {
    if (r) {
      ws.send('7');
    }
  }

  var last_enable_msg = '';
  function send_enable(d) {
    var e = '0';
    if (d && enable) {
      e = '1';
    }
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
  setInterval(function() {
    var controller = navigator.getGamepads()[0];
    if (controller == null) {
      $("#joystick-status").text("Disconnected");
      $("#joystick-status").removeClass("status-positive");
      $("#joystick-status").addClass("status-negative");


      send_movement(0, 0, 0);
      send_enable(0);
    } else {
      $("#joystick-status").text("Connected!");
      $("#joystick-status").removeClass("status-negative");
      $("#joystick-status").addClass("status-positive");

      send_movement(-controller.axes[1], -controller.axes[0], controller.axes[5]);
      send_enable(controller.buttons[0].pressed);
      reset_gyro(controller.buttons[7].pressed);
    }
  }, 20);
});

function pad(num, size) {
    var s = num+"";
    while (s.length < size) s = "0" + s;
    return s;
}
