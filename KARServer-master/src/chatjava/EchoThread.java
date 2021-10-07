package chatjava;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.Socket;

public class EchoThread extends Thread {

    protected Socket socket;
    public DataOutputStream out;
    private final int id;

    public EchoThread(Socket clientSocket, int id) {
        this.out = null;
        this.id = id;
        this.socket = clientSocket;
        System.out.println("Yeni istemci baglandi, id: " + id);
    }

    @Override
    public void run() {
        InputStream inp;
        BufferedReader brinp;

        try {
            inp = socket.getInputStream();
            brinp = new BufferedReader(new InputStreamReader(inp));
            out = new DataOutputStream(socket.getOutputStream());
        } catch (IOException e) {
            return;
        }
        String line;
        while (true) {
            try {
                line = brinp.readLine();
                if ((line == null) || line.equalsIgnoreCase("QUIT")) {
                    socket.close();
                    Server.threads.remove(this);
                    Server.index--;
                     System.out.println("Istemcı sunucudan ayrildi");
                    System.out.println("Istemcı threadsten silindi");
                    return;
                } else {
                    EchoThread otherClient = getOtherClient();
                    if (otherClient != null && Server.threads.size() > 1) {
                        otherClient.out.writeBytes(line + "\n");
                        otherClient.out.flush();
                    } else {
                        System.out.println("Veri alacak istemci yok");
                    }
                }
            } catch (IOException e) {
                Server.threads.remove(this);
                Server.index--;
                return;
            }
        }
    }

    private EchoThread getOtherClient() {
        EchoThread otherClient = null;
        for (EchoThread thread : Server.threads
        ) {
            if (this.id != thread.id) otherClient = thread;
        }
        return otherClient;
    }
}
