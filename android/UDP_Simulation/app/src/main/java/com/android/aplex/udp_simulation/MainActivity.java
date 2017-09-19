package com.android.aplex.udp_simulation;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;

public class MainActivity extends AppCompatActivity {

    private TempControlView tempControl;
    private int current_temp = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tempControl = (TempControlView) findViewById(R.id.temp_control);
        // 设置三格代表温度1度
        tempControl.setAngleRate(3);
        tempControl.setTemp(0, 100, 50);

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
                            address = InetAddress.getByName("23.106.155.16");

                            String send_str = "{\"temperature\":" + current_temp + "}";

                            DatagramPacket packet = new DatagramPacket(send_str.getBytes(), send_str.getBytes().length, address, 9098);
                            socket.send(packet);

                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });
    }
}
