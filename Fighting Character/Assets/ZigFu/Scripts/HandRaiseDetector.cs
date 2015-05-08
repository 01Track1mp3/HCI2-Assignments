using UnityEngine;
using System.Collections;

public class HandRaiseDetector : MonoBehaviour {
	public float threshhold = 2f;
	public GameObject leftHand;
	private bool leftHandRaised;
	public GameObject rightHand;
	private bool rightHandRaised;
	public GameObject head;
	private bool bothHandsRaised;
	
	public GameObject rightElbow;
	public GameObject leftElbow;
	
	private bool error;
	private TextMesh text;
	
	// Use this for initialization
	void Start () {
		leftHandRaised = false;
		rightHandRaised = false;
		text = gameObject.GetComponent<TextMesh>();
		
		error = false;
		if (leftHand == null) {
			print("Error: No left hand game object assigned.");
			error = true;
		}  
		if (rightHand == null) {
			print("Error: No right hand game object assigned.");
			error = true;
		}
		if (head == null) {
			print("Error: No head game object assigned.");
			error = true;
		}
	}
	
	// Update is called once per frame
	void Update () {
		checkHands();
	}
	
	void checkHands(){
		if (error)
			return;
		
		if (isRightArmRaised (threshhold) && isLeftArmRaised (threshhold)) {
			if (!bothHandsRaised) {
				bothHandsRaised = true;
				showNotifier ("Arms up");
			}
		} else if (!(isRightArmRaised(0) && isLeftArmRaised(0))){
			if (bothHandsRaised) {
				bothHandsRaised = false;
				showNotifier ("Arms down");
			}	
		}
		
		//		if (isRightHandRaised (threshhold) && isLeftHandRaised (threshhold)) {
		//			if (!bothHandsRaised) {
		//				bothHandsRaised = true;
		//				showNotifier ("Hands up");
		//			}
		//		} else if (!(isRightHandRaised(0) && isLeftHandRaised(0))){
		//			if (bothHandsRaised) {
		//				bothHandsRaised = false;
		//				showNotifier ("Hands down");
		//			}	
		//		}
		
		//		//check if left hand is raised
		//		if (leftHand.transform.position.y > head.transform.position.y + threshhold) {
		//
		//			if(!leftHandRaised){
		//				showNotifier("Left hand raised.");
		//				leftHandRaised = true;
		//			}
		//		} else if (leftHand.transform.position.y < head.transform.position.y){
		//			if(leftHandRaised) {
		//				showNotifier("Left hand lowered.");	
		//				leftHandRaised = false;
		//			}
		//		}
		//		//same for the right hand
		//		if (rightHand.transform.position.y > head.transform.position.y + threshhold) {	
		//			if(!rightHandRaised){
		//				showNotifier("Right hand raised.");
		//				rightHandRaised = true;
		//			}
		//		} else if (rightHand.transform.position.y < head.transform.position.y){
		//			if(rightHandRaised){
		//				showNotifier("Right hand lowered.");	
		//				rightHandRaised = false;
		//			}
		//		}
	}
	void showNotifier(string message){
		text.text = message;
	}
	
	bool isLeftHandRaised(float thresh) {
		return leftHand.transform.position.y > head.transform.position.y + thresh;
	}
	
	bool isRightHandRaised(float thresh) {
		return rightHand.transform.position.y > head.transform.position.y + thresh;
	}
	
	bool isLeftElbowRaised(float thresh) {
		return leftElbow.transform.position.y > head.transform.position.y + thresh;
	}
	
	bool isRightElbowRaised(float thresh) {
		return rightElbow.transform.position.y > head.transform.position.y + thresh;
	}
	
	bool isLeftArmRaised(float thresh) {
		return isLeftHandRaised (thresh) && isLeftElbowRaised (thresh);
	}
	
	bool isRightArmRaised(float thresh) {
		return isRightHandRaised (thresh) && isRightElbowRaised(thresh);
	}
	
}

