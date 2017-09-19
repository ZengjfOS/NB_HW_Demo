package com.android.aplex.udp_simulation;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;

public class MainActivity extends AppCompatActivity {

    private TempControlView tempControl;
    public EditText udp_ip;
    public EditText upd_port;
    private int current_temp = 0;

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

                            String send_str = "{\"temperature\":" + current_temp + "}";

                            DatagramPacket packet = new DatagramPacket(send_str.getBytes(), send_str.getBytes().length, address, Integer.valueOf(upd_port.getText().toString()));
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
