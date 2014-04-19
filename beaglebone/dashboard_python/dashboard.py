import thread
import time
import tornado.ioloop
import tornado.web
import tornado.websocket

from mbed_interface import MbedInterface


CLIENTS = {}
MI = MbedInterface()

class MainHandler(tornado.web.RequestHandler):
    @tornado.web.asynchronous
    def get(self):
        self.render('templates/index.html')


class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self, *args):
        self.id = self.get_argument("Id")
        self.stream.set_nodelay(True)
        CLIENTS[self.id] = {"id": self.id, "object": self}

    def on_message(self, message):
        """
        when we receive some message we want some message handler..
        for this example i will just print message to console
        """
        print "Client %s received a message : %s" % (self.id, message)

    def on_close(self):
        if self.id in CLIENTS:
            del CLIENTS[self.id]


application = tornado.web.Application([
    (r'/', MainHandler),
    (r'/ws', WebSocketHandler),
    (r'/css/(.*)', tornado.web.StaticFileHandler, {'path': 'static/css'}),
    (r'/js/(.*)', tornado.web.StaticFileHandler, {'path': 'static/js'}),
])


def sender_thread():
    while True:
        new_line = MI.get_line()
        if new_line is not None:
            for client in CLIENTS.values():
                client['object'].write_message(new_line)
        time.sleep(0.1)


if __name__ == "__main__":
    thread.start_new_thread(sender_thread, ())

    application.listen(8080)
    tornado.ioloop.IOLoop.instance().start()
