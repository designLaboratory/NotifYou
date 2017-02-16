package esapp.com.notifyoulight;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {
    Button b_phone_p;
    Button b_phone_m;
    Button b_sms_p;
    Button b_sms_m;
    Button b_mail_p;
    Button b_mail_m;
    Button b_mess_p;
    Button b_mess_m;
    Button b_fb_p;
    Button b_fb_m;
    Button b_start;
    Button b_reset;
    TextView t_phone_cnt;
    TextView t_sms_cnt;
    TextView t_mail_cnt;
    TextView t_mess_cnt;
    TextView t_fb_cnt;

    private BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
    private static String btAddress = "98:D3:31:30:46:AD";
    private static String btName = "notifYouSerialBt";
    //private static byte btPin[] = new byte[]{65, 66, 3, 4};
    private BluetoothDevice btNotifYouDev = null;
    private BluetoothSocket btSocket = null;
    private OutputStream btOutStream = null;
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private Set<BluetoothDevice> pairedDevices;
    private static byte notifCnt[] = new byte[]{0,0,0,0,0};


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        b_phone_p = (Button) findViewById(R.id.b_phone_p);
        b_phone_m = (Button) findViewById(R.id.b_phone_m);
        b_sms_p = (Button) findViewById(R.id.b_sms_p);
        b_sms_m = (Button) findViewById(R.id.b_sms_m);
        b_mail_p = (Button) findViewById(R.id.b_mail_p);
        b_mail_m = (Button) findViewById(R.id.b_mail_m);
        b_mess_p = (Button) findViewById(R.id.b_mess_p);
        b_mess_m = (Button) findViewById(R.id.b_mess_m);
        b_fb_p = (Button) findViewById(R.id.b_fb_p);
        b_fb_m = (Button) findViewById(R.id.b_fb_m);
        b_start= (Button) findViewById(R.id.b_start);
        b_reset= (Button) findViewById(R.id.b_reset);

        t_phone_cnt = (TextView) findViewById(R.id.t_phone_cnt);
        t_sms_cnt = (TextView) findViewById(R.id.t_sms_cnt);
        t_mail_cnt = (TextView) findViewById(R.id.t_mail_cnt);
        t_mess_cnt = (TextView) findViewById(R.id.t_mess_cnt);
        t_fb_cnt = (TextView) findViewById(R.id.t_fb_cnt);

        b_phone_p.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_inc(0);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_phone_m.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_dec(0);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_sms_p.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_inc(1);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_sms_m.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_dec(1);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_mail_p.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_inc(2);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_mail_m.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_dec(2);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_mess_p.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_inc(3);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_mess_m.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_dec(3);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_fb_p.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_inc(4);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_fb_m.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_dec(4);
                updateCntDisplay();
                sendBtData();
            }
        });
        b_start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                notifYouConnect(view);
                sendBtData();
            }
        });
        b_reset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                cnt_reset();
                updateCntDisplay();
                sendBtData();
            }
        });

    }

    public void notifYouConnect(View view) {
        if(btAdapter!= null) {
            if (!btAdapter.isEnabled()) {
                Intent enableBluetooth = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);

                while(!btAdapter.isEnabled());

                Toast.makeText(this, R.string.btRunning, Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(this, R.string.btEnable, Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(this, R.string.btNotSupport, Toast.LENGTH_SHORT).show();
        }
        pairedDevices= btAdapter.getBondedDevices();

        if(pairedDevices != null) {
            for(BluetoothDevice dev : pairedDevices){
                if(btName.equals(dev.getName())) {
                    btNotifYouDev = dev;
                    Toast.makeText(this, "notifYou has been find", Toast.LENGTH_SHORT).show();
                    break;
                }
            }
        }

        //textView.setText(btNotifYouDev.getAddress());

        try {
            btSocket = btNotifYouDev.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e1) {
            Toast.makeText(getBaseContext(), "ERROR - Could not create Bluetooth socket", Toast.LENGTH_SHORT).show();
        }

        // Establish the connection.
        try {
            btSocket.connect();
        } catch (IOException e) {
            try {
                btSocket.close();        //If IO exception occurs attempt to close socket
            } catch (IOException e2) {
                Toast.makeText(getBaseContext(), "ERROR - Could not close Bluetooth socket", Toast.LENGTH_SHORT).show();
            }
        }
        // Create a data stream so we can talk to the device
        try {
            btOutStream= btSocket.getOutputStream();
        } catch (IOException e) {
            Toast.makeText(getBaseContext(), "ERROR - Could not create bluetooth outstream", Toast.LENGTH_SHORT).show();
        }

    }

    public void cnt_inc(int notifPtr) {
        if (notifPtr >= 0 && notifPtr < notifCnt.length) {
            notifCnt[notifPtr]+= 1;
        }
    }

    public void cnt_dec(int notifPtr) {
        if (notifPtr >= 0 && notifPtr < notifCnt.length) {
            if(notifCnt[notifPtr] > 0)
                notifCnt[notifPtr]-= 1;
        }
    }

    public void cnt_reset() {
        //for (int i : notifCnt){
        for(int i= 0; i < 5; i++){
            notifCnt[i]= 0;
        }
    }

    public void sendBtData() {
        //for (int i : notifCnt) {
        for(int i= 0; i < 5; i++){
            try {
                //attempt to place data on the outstream to the BT device
                btOutStream.write(notifCnt[i]);
            } catch (IOException e) {
                //if the sending fails this is most likely because device is no longer there
                Toast.makeText(getBaseContext(), "ERROR - Device not found", Toast.LENGTH_SHORT).show();
                //finish();
            }
        }
    }

    public void updateCntDisplay(){

        t_phone_cnt.setText("" + notifCnt[0]);
        t_sms_cnt.setText("" + notifCnt[1]);
        t_mail_cnt.setText("" + notifCnt[2]);
        t_mess_cnt.setText("" + notifCnt[3]);
        t_fb_cnt.setText("" + notifCnt[4]);
    }
}