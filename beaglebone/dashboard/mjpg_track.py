import copy
import cv2
import numpy as np
import urllib

stream = urllib.urlopen('http://127.0.0.1:8090/?action=stream')
AREA_THRESH = 10

def detect_target(frame):
    # Convert BGR to HSV
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

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
        print area
        if area > AREA_THRESH and area > largest_area:
            largest_area = area
            largest_cnt = cnt

    if largest_cnt is not None:
        (x, y), radius = cv2.minEnclosingCircle(largest_cnt)
        center = (int(x), int(y))
        print center
        radius = int(radius)
        cv2.circle(frame, center, radius, (0, 0, 255), thickness=10)

    return mask

bytes=''
while True:
    bytes += stream.read(1024)
    a = bytes.find('\xff\xd8')
    b = bytes.find('\xff\xd9')
    if a != -1 and b != -1:
        jpg = bytes[a:b+2]
        bytes = bytes[b+2:]
        frame = cv2.imdecode(np.fromstring(jpg, dtype=np.uint8),cv2.CV_LOAD_IMAGE_COLOR)
  
        mask = detect_target(frame)

        # cv2.imshow('frame', frame)
        # cv2.imshow('mask', mask)

    #if cv2.waitKey(1) == 27:
    #    exit(0)

cv2.destroyAllWindows()


