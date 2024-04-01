package com.panzq.applicationb;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.PersistableBundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.net.SocketAddress;

public class TcpMainActivity extends AppCompatActivity implements SafeHandler.Callback {


    LocalSocket lsocket = null;
    BufferedWriter br;
    Handler handler;
    EditText sendCmdText = null;
    TextView receiveText = null;
    private static final int HANDLER_SENDMESSAGE = 0x100;

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    public void onSaveInstanceState(Bundle outState, PersistableBundle outPersistentState) {
        super.onSaveInstanceState(outState, outPersistentState);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
    }


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.tcp_activity_main);
        Button starActivityA = findViewById(R.id.send);
        sendCmdText = findViewById(R.id.send_cmd);
        receiveText = findViewById(R.id.receiveText);
        HandlerThread handlerThread = new HandlerThread("mythread");
        handlerThread.start();
        handler = new Handler(handlerThread.getLooper());

        starActivityA.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i("test","rootclient send cmd to rootServer cmd = " + sendCmdText.getText().toString());
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        sendMessage();
                    }
                }).start();
//                Bundle bundle = getContentResolver().call(Uri.parse("content://com.android.shell.execute"),"cmd",sendCmdText.getText().toString(),null);
//                if (bundle!= null) {
//                    String result = bundle.getString("result");
//                    receiveText.setText(result);
//                }
            }
        });
        Button provier = findViewById(R.id.provider);
        provier.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    Log.i("test", "Runtime.getRuntime().exec call arg = " + sendCmdText.getText().toString());
                    Process p = Runtime.getRuntime().exec(sendCmdText.getText().toString());
                    BufferedReader in = new BufferedReader(new InputStreamReader(p.getInputStream()));
                    String line = null;
                    String str = null;
                    while ((line = in.readLine()) != null) {
                        str += line + "\n";
                        Log.i("test", "Runtime.getRuntime().exec resultline = " + line);
                    }
                   receiveText.setText(str);
                } catch (Exception e) {
                    e.printStackTrace();
                    Log.i("test", "Runtime.getRuntime().exec Exception " ,e);
                }


            }
        });
    }

    private void sendMessage() {
        try {
            lsocket = new LocalSocket();
            LocalSocketAddress address = new LocalSocketAddress("server-socket", LocalSocketAddress.Namespace.ABSTRACT);
            lsocket.connect(address);
            String result;
            Log.i("test","rootclient send cmd to rootServer cmd = " + sendCmdText.getText().toString());
            br = new BufferedWriter(new OutputStreamWriter(lsocket.getOutputStream()));
            br.write(sendCmdText.getText().toString());
            br.newLine();
            br.flush();
//            br.close();

            Log.d("test", "========发送成功========");
            InputStream inputStream = lsocket.getInputStream();
            byte[] buf = new byte[inputStream.available() -1];
            inputStream.read(buf,0,inputStream.available() -1) ;
            String str =new String(buf);
            Log.d("test", "========接受成功======== inputStream.available() = " +inputStream.available() + "read str  = " +str);
            final String str1 = str;
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    receiveText.setText(str1);
                }
            });
            lsocket.close();
        } catch (IOException e) {
            e.printStackTrace();
            Log.i("test","rootclient rootServer 发送失败",e);
        }
    }

    @Override
    public void handlerMessage(Message msg) {
        switch (msg.what) {
            case HANDLER_SENDMESSAGE:
                sendMessage();
                break;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);

    }

}
