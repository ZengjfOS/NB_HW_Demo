package com.android.aplex.udp_simulation;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.JsonReader;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;

public class MainActivity extends AppCompatActivity {

    private TempControlView tempControl;
    public EditText udp_ip;
    public EditText upd_port;
    private int current_temp = 0;
    public Activity activity = this;
    public String sendback;
    public int temperature;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        udp_ip = (EditText) (findViewById(R.id.udp_ip));
        upd_port = (EditText) (findViewById(R.id.udp_port));

        tempControl = (TempControlView) findViewById(R.id.temp_control);
        // 设置三格代表温度1度
        tempControl.setAngleRate(3);
        tempControl.setTemp(0, 100, 50);
        tempControl.setTemp(50);

        tempControl.setOnTempChangeListener(new TempControlView.OnTempChangeListener() {
            @Override
            public void change(int temp) {
                Toast.makeText(MainActivity.this, temp + "°", Toast.LENGTH_SHORT).show();

                current_temp = temp;

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        DatagramSocket socket;
                        InetAddress address;

                        try {
                            socket = new DatagramSocket();
                            address = InetAddress.getByName(udp_ip.getText().toString());
                            Log.e("MainActivity", udp_ip.getText().toString());

                            String send_str = "{\"temperature\":" + current_temp + "}";

                            DatagramPacket packet = new DatagramPacket(send_str.getBytes(), send_str.getBytes().length, address, Integer.valueOf(upd_port.getText().toString()));
                            socket.send(packet);

                            socket.setSoTimeout(1000 * 6);
                            byte[] message = new byte[128];
                            DatagramPacket p = new DatagramPacket(message, message.length);
                            socket.receive(p);
                            sendback = new String(message, 0, p.getLength());

                            /*
                            activity.runOnUiThread(new Runnable() {
                                public void run() {
                                    Toast.makeText(activity, sendback, Toast.LENGTH_SHORT).show();
                                }
                            });
                            */

                            Log.d("Udp tutorial","message:" + sendback);

                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });

        new Thread(new Runnable() {
            @Override
            public void run() {
                DatagramSocket socket;
                InetAddress address;

                while(true) {
                    try {
                        socket = new DatagramSocket();
                        address = InetAddress.getByName(udp_ip.getText().toString());

                        String send_str = "refresh";
                        Log.d("MainActivity",  send_str);

                        DatagramPacket packet = new DatagramPacket(send_str.getBytes(), send_str.getBytes().length, address, Integer.valueOf(upd_port.getText().toString()));
                        socket.send(packet);

                        socket.setSoTimeout(1000 * 10);
                        byte[] message = new byte[128];
                        DatagramPacket p = new DatagramPacket(message, message.length);
                        socket.receive(p);
                        sendback = new String(message, 0, p.getLength());
                        Log.d("MainActivity", sendback);

                        if (sendback.length() > 0) {
                            InputStream json = new ByteArrayInputStream(sendback.getBytes());
                            JsonReader reader = new JsonReader(new InputStreamReader(json));

                            try {
                                reader.beginObject();
                                while (reader.hasNext()) {
                                    String keyName = reader.nextName();
                                    if ("temperature".equals(keyName)) {
                                        temperature = reader.nextInt();
                                        Log.e("MainActivity", "wendu" + temperature);
                                        activity.runOnUiThread(new Runnable() {
                                            public void run() {
                                                tempControl.setTemp(temperature);
                                            }
                                        });
                                    }
                                }
                                reader.endObject();
                            } finally {
                                reader.close();
                            }

                        }

                        Thread.sleep(1000);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();
    }
}
