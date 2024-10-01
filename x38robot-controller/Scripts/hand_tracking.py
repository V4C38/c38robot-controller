import cv2
import mediapipe as mp
import math
from collections import deque

# Initialize MediaPipe hands model
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(min_detection_confidence=0.5, min_tracking_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

# Gesture settings
active_count = 0
inactive_count = 0
debounce_threshold = 5
distance_threshold = 0.3
frame_rate = 30  # Adjust this based on your actual camera frame rate
trail_length = int(frame_rate * 0.5)  # Adjust to keep trail for 0.5 seconds
hand_trail = deque(maxlen=trail_length)
depth_values = deque(maxlen=trail_length)

def distance_between_points(p1, p2):
    """Calculate Euclidean distance between two normalized 3D points."""
    return math.sqrt((p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2 + (p1.z - p2.z) ** 2)

def apply_hand_tracking(frame):
    global active_count, inactive_count, hand_trail, depth_values

    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(rgb_frame)
    midpoint_coordinates = (0.0, 0.0, 0.0)

    if result.multi_hand_landmarks:
        for hand_landmarks in result.multi_hand_landmarks:
            index_tip = hand_landmarks.landmark[8]
            thumb_tip = hand_landmarks.landmark[4]
            distance = distance_between_points(index_tip, thumb_tip)

            if distance < distance_threshold:
                active_count += 1
                inactive_count = 0
            else:
                inactive_count += 1
                active_count = 0

            if active_count >= debounce_threshold:
                midpoint_x = (index_tip.x + thumb_tip.x) / 2
                midpoint_y = (index_tip.y + thumb_tip.y) / 2
                midpoint_z = (index_tip.z + thumb_tip.z) / 2
                midpoint_coordinates = (midpoint_x, midpoint_y, midpoint_z)
                hand_trail.append((int(midpoint_x * frame.shape[1]), int(midpoint_y * frame.shape[0])))
                depth_values.append(midpoint_z)
                
                # Draw a green dot at the midpoint for visualization
                cv2.circle(frame, (int(midpoint_x * frame.shape[1]), int(midpoint_y * frame.shape[0])), 10, (0, 255, 0), -1)
            else:
                midpoint_coordinates = (0.0, 0.0, 0.0)

            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

    # Draw the hand trail
    if hand_trail:
        for i in range(1, len(hand_trail)):
            # Dynamic thickness based on depth: More depth (closer) -> thicker line
            thickness = max(int(depth_values[i] * 50), 1)  # Scale factor might need adjustment
            cv2.line(frame, hand_trail[i-1], hand_trail[i], (0, 255, 255), thickness)

    return frame, midpoint_coordinates
