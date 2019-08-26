package com.internet;

import java.net.InetAddress;

import java.io.*;
import java.net.*;

public class Internet {
	static InetAddress ip;
	private ServerSocket server;
	private BufferedReader reader;
	private Socket socket;

	void createServer() {
		try {
			server = new ServerSocket(8000);
			while(true) {
				socket = server.accept();
				System.out.println("connecting...");
				reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				while(true) {
					System.out.println(reader.readLine());
				}
			}
		}catch (Exception e) {
			e.printStackTrace();
		}
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try {
			ip = InetAddress.getLocalHost();
			System.out.println(ip.getHostAddress());
			System.out.println(ip.getHostName());
			
			Internet tcpServer = new Internet();
			tcpServer.createServer();
		}catch (Exception e) {
			e.printStackTrace();
		}

	}

}
