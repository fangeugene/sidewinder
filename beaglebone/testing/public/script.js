var ws = new WebSocket('ws://192.168.1.100:8081');

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

    $('#module0power').attr('r', Math.log(parseInt(split_msg[1])));
    $('#module1power').attr('r', Math.log(parseInt(split_msg[3])));
    $('#module2power').attr('r', Math.log(parseInt(split_msg[5])));
  }
};
ws.onclose = function() { 
  // websocket is closed.
  console.log("Connection is closed..."); 
};

$(document).ready(function(){
  var ledon = false;
  $('#button1').click(function() {
    if (ledon) {
      ws.send('0 0');
      ledon = false;
    } else {
      ws.send('0 1');
      ledon = true;
    }
  });

  $("#joystick").on("mousemove", $.throttle(20, function(e) {
    if (e.which == 1) {
      var y = parseFloat(e.pageX - this.offsetLeft);
      var x = parseFloat(e.pageY - this.offsetTop);
      x -= 100;
      y -= 100;
      x *= -1;
      y *= -1;

      var r = Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2));
      var theta = Math.atan(y/x);
      if (x < 0) {
        theta += Math.PI;
      }
      if (theta > Math.PI) {
        theta -= 2 * Math.PI;
      }
      theta = theta * 180 / Math.PI;
      ws.send('1 ' + pad(parseInt(r), 3) +  ' ' + pad(parseInt(theta+500), 3));
    }
  }));
});

function pad(num, size) {
    var s = num+"";
    while (s.length < size) s = "0" + s;
    return s;
}
