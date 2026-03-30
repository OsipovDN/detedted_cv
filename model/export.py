from ultralytics import YOLO

model = YOLO("yolov8n.pt")  # load the trained model

model.export(
    format = "tflite", 
)