package tcpClient;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;


public class tcpClient {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try {
			Socket socket = new Socket("192.168.175.1",8000);
			OutputStream  os =  socket.getOutputStream();
			OutputStreamWriter oswrite = new OutputStreamWriter(os);
			BufferedWriter writer1 = new BufferedWriter(oswrite);
			PrintWriter writer = new PrintWriter(os,true);
			while(true)
			{
				writer.println("printwriter client");
				writer1.write("bufferwriter client\n");
				writer1.newLine();
				writer1.flush();
				Thread.sleep(1000);
			}

		}catch(Exception e) {
			e.printStackTrace();
		}

	}

}
