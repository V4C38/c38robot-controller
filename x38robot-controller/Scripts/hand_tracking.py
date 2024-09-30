import cv2
import mediapipe as mp
import math

# Initialize MediaPipe hands model
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(min_detection_confidence=0.5, min_tracking_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

def distance_between_points(p1, p2):
    # Calculate Euclidean distance between two normalized 3D points (index tip and thumb tip)
    # We are using normalized coordinates (x, y, z)
    return math.sqrt((p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2 + (p1.z - p2.z) ** 2)


def apply_hand_tracking(frame):
    # Convert the frame to RGB as required by MediaPipe
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(rgb_frame)

    midpoint_coordinates = (0.0, 0.0, 0.0)  # Default if no hand detected
    if result.multi_hand_landmarks:
        for hand_landmarks in result.multi_hand_landmarks:
            
            index_tip = hand_landmarks.landmark[8]
            thumb_tip = hand_landmarks.landmark[4]
            
            distance = distance_between_points(index_tip, thumb_tip)
            distance_threshold = 0.2

            # If the normalized distance is less than the threshold, compute the midpoint
            if distance < distance_threshold:
                midpoint_x = (index_tip.x + thumb_tip.x) / 2
                midpoint_y = (index_tip.y + thumb_tip.y) / 2
                midpoint_z = (index_tip.z + thumb_tip.z) / 2
                midpoint_coordinates = (midpoint_x, midpoint_y, midpoint_z)

                # Draw a circle at the midpoint for visualization
                h, w, _ = frame.shape
                cv2.circle(frame, (int(midpoint_x * w), int(midpoint_y * h)), 10, (0, 255, 0), -1)  # Green dot
            else:
                midpoint_coordinates = (0.0, 0.0, 0.0)

            # Draw hand landmarks on the frame (for visualization)
            mp.solutions.drawing_utils.draw_landmarks(
                frame, hand_landmarks, mp_hands.HAND_CONNECTIONS
            )

    return frame, midpoint_coordinates