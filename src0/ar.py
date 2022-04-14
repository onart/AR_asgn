import cv2
import matplotlib.pyplot as plt
import numpy as np

if __name__=='__main__':
    markerSet=cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_250)
    img=cv2.imread('ar.png')
    detecteds, ids, rejecteds=cv2.aruco.detectMarkers(img,markerSet)
    cv2.aruco.drawDetectedMarkers(img,detecteds,ids)
    plt.imshow(img)
    plt.show()

    cm=np.array([[1,0,0],[0,1,0],[0,0,1]])
    cv2.aruco.estimatePoseSingleMarkers(detecteds,0.05,cm,1)
