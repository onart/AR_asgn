import cv2
import matplotlib.pyplot as plt
import numpy as np
import random

if __name__=='__main__':
    markerSet=cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_250)
    brd=cv2.imread('./dict_1.png')
    a,b,c=cv2.aruco.detectMarkers(brd,markerSet)


