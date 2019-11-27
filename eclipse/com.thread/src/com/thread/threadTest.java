package com.thread;

import java.awt.BorderLayout;

import javax.swing.JFrame;
import javax.swing.JProgressBar;

public  class threadTest extends JFrame{
	
	public static class threadSync implements Runnable{
		int num = 10;
		public void run() {
			while(true) {
				synchronized ("") {
					if(num > 0) {
						try {
							Thread.sleep(1000);
						}catch(Exception e) {
							e.printStackTrace();
						}
						System.out.println("num value:"+num--);
					}
				}
			}
		}
	}
	
	private Thread threadA;
	private Thread threadB;
	final JProgressBar barA = new JProgressBar();
	final JProgressBar barB = new JProgressBar();
	int count = 0;

	public static void main(String[] args) {
		// TODO Auto-generated method stub
//		init(new threadTest(),100,100);
		threadSync T = new threadSync();
		Thread a = new Thread(T);
		Thread b = new Thread(T);
		Thread c = new Thread(T);
		Thread d = new Thread(T);
		a.start();
		b.start();
		c.start();
		d.start();
		
	}
	public static void init(JFrame frame, int width, int height) {
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(width, height);
		frame.setVisible(true);
	}
	public threadTest() {
		super();
		barA.setStringPainted(true);
		barB.setStringPainted(true);
		getContentPane().add(barA,BorderLayout.NORTH);
		getContentPane().add(barB,BorderLayout.SOUTH);
		threadA = new Thread(new Runnable() {
			int count = 0;
			public void run() {
				while(true) {
					barA.setValue(count++);
					try{
						Thread.sleep(100);
						threadB.join();
					}catch (Exception e) {
						e.printStackTrace();
					}

				}
			}

		});
		threadA.start();

		threadB = new Thread(new Runnable() {
			int count = 0;
			public void run() {
				while(true) {
					barB.setValue(count++);
					try{
						Thread.sleep(100);
					}catch (Exception e) {
						e.printStackTrace();
					}
					if(count == 100)
						break;
				}
			}

		});
		threadB.start();
	}

}
