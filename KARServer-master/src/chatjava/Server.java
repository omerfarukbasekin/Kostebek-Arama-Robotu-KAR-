package chatjava;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;

public class Server {

    static final int PORT = 5000;
    public static ArrayList<EchoThread> threads;
    public static int index;

    public static void main(String... args) {
        ServerSocket serverSocket = null;
        Socket socket = null;
        threads = new ArrayList<>();
        index = 0;
        try {
            serverSocket = new ServerSocket(PORT);
            System.out.println(serverSocket);
        } catch (IOException e) {
            System.out.println("Hata: " + e);
        }
        while (true) {
            try {
                socket = serverSocket.accept();
            } catch (IOException e) {
                System.out.println("I/O hata: " + e);
            }
            EchoThread newClient = new EchoThread(socket, index);
            threads.add(newClient);
            newClient.start();

            Server.index++;
        }
    }
}
