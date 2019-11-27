import QtQuick 2.7

Rectangle {
    width: 100
    height: 100
    color: "green"
    XAnimator on x {								//(b)
        from: 10;
        to: 100;
        duration: 7000
        loops: Animator.Infinite
    }
    YAnimator on y {								//(c)
        from: 10;
        to: 100;
        duration: 7000
        loops: Animator.Infinite
    }
    ScaleAnimator on scale {						//(d)
        from: 0.1;
        to: 1;
        duration: 7000
        loops: Animator.Infinite
    }
    RotationAnimator on rotation {					//(e)
        from: 0;
        to: 360;
        duration:7000
        loops: Animator.Infinite
    }
    OpacityAnimator on opacity {					//(f)
        from: 0;
        to: 1;
        duration: 7000
        loops: Animator.Infinite
    }
}

