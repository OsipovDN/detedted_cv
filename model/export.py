from ultralytics import YOLO

model = YOLO("yolov8n.pt")  # load the trained model

model.export(
    format = "onnx", 
    imgsz = [640,640], 
    opset = 12, 
    dynamic = False, 
    simplify = True
)