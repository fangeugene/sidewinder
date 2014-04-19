import copy
import cv2
import cv2.cv as cv
import numpy as np

AREA_THRESH = 5


class TargetTracker(object):
    def __init__(self):
        self.cap = cv2.VideoCapture(0)
        self.cap.set(cv.CV_CAP_PROP_FPS, 30)
        self.cap.set(cv.CV_CAP_PROP_FRAME_WIDTH, 400)
        self.cap.set(cv.CV_CAP_PROP_FRAME_HEIGHT, 300)


    def detect_target(self, frame):
        small = cv2.resize(frame, (0,0), fx=0.2, fy=0.2) 

        # Convert BGR to HSV
        hsv = cv2.cvtColor(small, cv2.COLOR_BGR2HSV)

        # define range of desired color in HSV
        lower_thresh = np.array([60, 100, 220], dtype=np.uint8)
        upper_thresh = np.array([100, 255, 255], dtype=np.uint8)

        # Threshold the HSV image to get only desired colors
        mask = cv2.inRange(hsv, lower_thresh, upper_thresh)

        contours, _ = cv2.findContours(copy.copy(mask), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        largest_area = 0
        largest_cnt = None
        for cnt in contours:
            area = cv2.contourArea(cnt)
            if area > AREA_THRESH and area > largest_area:
                largest_area = area
                largest_cnt = cnt

        if largest_cnt is not None:
            (x, y), radius = cv2.minEnclosingCircle(largest_cnt)
            center = (int(x), int(y))
            return center
            # radius = int(radius)
            # cv2.circle(frame, center, radius, (0, 0, 255), thickness=10)

        return None

    def get_coordinates(self):
        while(True):
            print '--'
            # Take each frame
            _, frame = self.cap.read()

            print self.detect_target(frame)

            # cv2.waitKey(1)
            # cv2.imshow('frame', frame)
            # cv2.imshow('mask', mask)

            # if cv2.waitKey(1) == 27:
            #     exit(0)



    # cv2.destroyAllWindows()
    # self.cap.release()

t = TargetTracker()
t.get_coordinates()

