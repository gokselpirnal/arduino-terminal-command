package com.gokselpirnal.bluetoothterminal;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.graphics.Typeface;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.method.ScrollingMovementMethod;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import app.akexorcist.bluetotohspp.library.BluetoothSPP;
import app.akexorcist.bluetotohspp.library.BluetoothState;
import app.akexorcist.bluetotohspp.library.DeviceList;
import mehdi.sakout.fancybuttons.FancyButton;

public class MainActivity extends AppCompatActivity {
    FancyButton turnOnOff, connectBtn;
    FancyButton sendBtn;
    TextView deviceInfo, history;
    EditText dataStr;
    BluetoothSPP bt;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        turnOnOff = (FancyButton) findViewById(R.id.turn_on_off);
        connectBtn = (FancyButton) findViewById(R.id.connect);
        deviceInfo = (TextView) findViewById(R.id.device_info);
        history = (TextView) findViewById(R.id.history);
        sendBtn = (FancyButton) findViewById(R.id.send);
        dataStr = (EditText) findViewById(R.id.data);

        Typeface type = Typeface.createFromAsset(getAssets(), "fonts/MesloLGL-Regular.ttf");
        history.setTypeface(type);
        dataStr.setTypeface(type);

        history.setMovementMethod(new ScrollingMovementMethod());

        setup();

        bt.setBluetoothConnectionListener(new BluetoothSPP.BluetoothConnectionListener() {
            public void onDeviceConnected(String name, String address) {
                deviceInfo.setText(name + " : " + address);
            }

            public void onDeviceDisconnected() {
                deviceInfo.setText("Bağlantı koptu!");
            }

            public void onDeviceConnectionFailed() {
                deviceInfo.setText("Bağlantı kurulamadı!");
            }
        });

        sendBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                send();
            }
        });

        connectBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                if (bt.getServiceState() == BluetoothState.STATE_CONNECTED) {
                    bt.disconnect();
                } else {
                    Intent intent = new Intent(getApplicationContext(), DeviceList.class);
                    intent.putExtra("bluetooth_devices", "Bluetooth Cihazları");
                    intent.putExtra("no_devices_found", "Cihaz bulunamadı!");
                    intent.putExtra("scanning", "Taranıyor");
                    intent.putExtra("scan_for_devices", "Tara");
                    intent.putExtra("select_device", "Seç");
                    startActivityForResult(intent, BluetoothState.REQUEST_CONNECT_DEVICE);
                }
            }
        });


        turnOnOff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (!bt.isBluetoothEnabled()) {
                    Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(turnOn, 0);
                } else {
                    bt.getBluetoothAdapter().disable();
                }
            }
        });

        dataStr.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView textView, int i, KeyEvent keyEvent) {
                if (i == EditorInfo.IME_ACTION_SEND) {
                    send();
                    return true;
                }
                return false;
            }
        });

    }

    private void send() {
        String data = dataStr.getText().toString();

        if (data.length() > 0) {
            bt.send(dataStr.getText().toString() + "\n", false);
            history.append("$ " + dataStr.getText().toString() + "\n");
            history.scrollTo(0, history.getLayout().getLineTop(history.getLineCount()) - history.getHeight());
            dataStr.setText("");
        }
    }

    public void setup() {
        bt = new BluetoothSPP(getApplicationContext());

        if (!bt.isBluetoothAvailable()) {
            Toast.makeText(getApplicationContext(), "Bluetooth mevcut değil!", Toast.LENGTH_SHORT).show();
            finish();
        }

        if (!bt.isBluetoothEnabled()) {
            bt.enable();
        }
        bt.setupService();
        bt.startService(false);
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == BluetoothState.REQUEST_CONNECT_DEVICE) {
            if (resultCode == Activity.RESULT_OK) {
                deviceInfo.setText("Bağlanılıyor...");
                bt.connect(data);
            }
        }
    }

}
