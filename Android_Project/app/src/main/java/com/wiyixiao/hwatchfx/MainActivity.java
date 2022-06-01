package com.wiyixiao.hwatchfx;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.text.format.Time;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.vise.baseble.ViseBle;
import com.vise.baseble.callback.IConnectCallback;
import com.vise.baseble.callback.scan.IScanCallback;
import com.vise.baseble.callback.scan.ScanCallback;
import com.vise.baseble.core.DeviceMirror;
import com.vise.baseble.exception.BleException;
import com.vise.baseble.model.BluetoothLeDevice;
import com.vise.baseble.model.BluetoothLeDeviceStore;
import com.vise.log.ViseLog;
import com.wiyixiao.hwatchfx.adapter.AlarmListAdapter;
import com.wiyixiao.hwatchfx.base.BaseActivity;
import com.wiyixiao.hwatchfx.bean.AlarmBean;
import com.wiyixiao.hwatchfx.bean.EnvBean;
import com.wiyixiao.hwatchfx.bean.TimeBean;
import com.wiyixiao.hwatchfx.blecore.BleConstants;
import com.wiyixiao.hwatchfx.blecore.BleManager;
import com.wiyixiao.hwatchfx.blecore.IBleInterface;
import com.wiyixiao.hwatchfx.blecore.IDeviceSearchListener;
import com.wiyixiao.hwatchfx.core.LocalThreadPools;
import com.wiyixiao.hwatchfx.core.PriorityRunnable;
import com.wiyixiao.hwatchfx.core.PriorityType;
import com.wiyixiao.hwatchfx.dialog.DeviceDialog;
import com.wiyixiao.hwatchfx.utils.Permission;
import com.wiyixiao.hwatchfx.utils.Utils;

import java.util.ArrayList;
import java.util.Calendar;

public class MainActivity extends BaseActivity {
    private static final int CODE_TIME_UPDATE = 0X00;

    private MenuItem menuItemConn;
    private DeviceDialog deviceDialog;
    private ProgressDialog progressDialog;
    private TextView pressTv, thTv, sysTimTv;
    private ListView alarmLv;
    private Button timSetBtn, alarmReadBtn, alarmClearBtn;

    private AlarmListAdapter alarmListAdapter;
    private ArrayList<AlarmBean> alarmBeanArrayList = new ArrayList<>();

    private TimeBean timeBean = new TimeBean();
    private CommManager commManager = new CommManager();

    private boolean activityExitFlag = false;

    private Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(@NonNull Message msg) {
            Bundle bundle = msg.getData();
            switch (msg.what) {
                case CODE_TIME_UPDATE:
                    @SuppressLint("DefaultLocale")
                    String time_str = String.format("%04d/%02d/%02d %02d:%02d:%02d %s",
                            timeBean.getYear(),
                            timeBean.getMonth(),
                            timeBean.getDay(),
                            timeBean.getHour(),
                            timeBean.getMin(),
                            timeBean.getSec(),
                            timeBean.getWeekStr());
                    sysTimTv.setText(time_str);
                    break;
            }
            return false;
        }
    });

    private Runnable timeUpdateRunnable = new Runnable() {
        @Override
        public void run() {
            while (!activityExitFlag) {
                Calendar calendar = Calendar.getInstance();
                timeBean.setYear(calendar.get(Calendar.YEAR));
                timeBean.setMonth(calendar.get(Calendar.MONTH) + 1);
                timeBean.setDay(calendar.get(Calendar.DAY_OF_MONTH));
                timeBean.setHour(calendar.get(Calendar.HOUR_OF_DAY));
                timeBean.setMin(calendar.get(Calendar.MINUTE));
                timeBean.setSec(calendar.get(Calendar.SECOND));
                timeBean.setWeek(calendar.get(Calendar.DAY_OF_WEEK) - 1);

                Message message = handler.obtainMessage();
                message.what = CODE_TIME_UPDATE;
                message.sendToTarget();

                try {
                    Thread.sleep(1000);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    };

    private IBleInterface bleInterface = new IBleInterface() {
        @Override
        public void toastMsg(String msg) {
            Toast.makeText(getmContext(), msg, Toast.LENGTH_SHORT).show();
        }

        @SuppressLint("DefaultLocale")
        @Override
        public void recvCall(byte[] data) {
//            ViseLog.e(data.length + "-" + Utils.bytesToHex(data));
            //解析数据
            byte state = commManager.checkData(data);
            switch (state){
                case CommManager.CommState.env_upload: //环境数据上传
                    commManager.parseEnvData(data);
                    //设置大气压
                    pressTv.setText(String.format("%.2f hPa", commManager.getEnvBean().getPress()));
                    //设置温湿度
                    thTv.setText(String.format("%.2f°C  %.2f%%", commManager.getEnvBean().getTemp(), commManager.getEnvBean().getHumi()));
                    break;
                case CommManager.CommState.alarm_upload: //闹钟列表上传

                    break;
            }
        }
    };

    private ScanCallback scanCallback = new ScanCallback(new IScanCallback() {
        @Override
        public void onDeviceFound(BluetoothLeDevice bluetoothLeDevice) {
            ViseLog.i("BLE device name:mac: %s:%s", bluetoothLeDevice.getName(), bluetoothLeDevice.getAddress());
            if (deviceDialog != null) deviceDialog.addDevice(bluetoothLeDevice);
        }

        @Override
        public void onScanFinish(BluetoothLeDeviceStore bluetoothLeDeviceStore) {
            ViseLog.i("BLE scan finish");
        }

        @Override
        public void onScanTimeout() {
            ViseLog.i("BLE scan timeout");
        }
    });

    private IConnectCallback connectCallback = new IConnectCallback() {
        @Override
        public void onConnectSuccess(DeviceMirror deviceMirror) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ViseLog.d("BLE Addr: %s", BleManager.getInstance().getBleDeivce().getAddress());
                    menuItemConn.setTitle("断开");
                    if (progressDialog != null) progressDialog.dismiss();
                }
            });
            BleManager.getInstance().getDeviceUUID();
            BleManager.getInstance().bleWritePack(commManager.getConnBytes()); //发送连接成功命令
            toastInfoUiThread("设备连接成功");

        }

        @Override
        public void onConnectFailure(BleException exception) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {

                }
            });
            toastInfoUiThread("设备连接失败");
        }

        @Override
        public void onDisconnect(boolean isActive) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    menuItemConn.setTitle("连接");
                }
            });
            toastInfoUiThread("设备断开连接");
        }
    };

    private IDeviceSearchListener deviceSearchListener = new IDeviceSearchListener() {
        @Override
        public void deviceSelected(BluetoothLeDevice device) {
            ViseLog.i("%s\n%s", device.getName(), device.getAddress());
            ViseBle.getInstance().stopScan(scanCallback); //停止扫描
            BleManager.getInstance().setBleDeivce(device);

            if (progressDialog != null) {
                progressDialog.setMessage(device.getAddress() + " 连接中...");
                progressDialog.show();
            }
            //修改mtu
            //修改成功后再连接
            setMtu();
        }

        @Override
        public void deviceScanCancel() {
            ViseBle.getInstance().stopScan(scanCallback); //停止扫描
        }
    };

    private View.OnClickListener clickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            switch (view.getId()) {
                case R.id.time_set_btn:
                    //设置时间
                    ViseLog.d(Utils.bytesToHex(commManager.getTimeBytes(timeBean)));
                    BleManager.getInstance().bleWritePack(commManager.getTimeBytes(timeBean));
                    break;
                case R.id.read_alarm_btn:
                    Toast.makeText(getmContext(),"功能未添加", Toast.LENGTH_SHORT).show();
                    break;
                case R.id.clear_alarm_btn:
                    Toast.makeText(getmContext(),"功能未添加", Toast.LENGTH_SHORT).show();
                    if (alarmBeanArrayList.size() > 0) {
                        alarmBeanArrayList.clear();
                        alarmListAdapter.notifyDataSetChanged();
                    }
                    break;
                default:
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //初始化控件
        deviceDialog = new DeviceDialog(this);
        deviceDialog.setDeviceSearchListener(deviceSearchListener);

        progressDialog = new ProgressDialog(this);
        progressDialog.setTitle("设备连接");
        progressDialog.setMessage("连接中...");
        progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);

        pressTv = findViewById(R.id.env_press_tv);
        thTv = findViewById(R.id.env_th_tv);
        sysTimTv = findViewById(R.id.sys_tim_tv);
        timSetBtn = findViewById(R.id.time_set_btn);
        alarmReadBtn = findViewById(R.id.read_alarm_btn);
        alarmClearBtn = findViewById(R.id.clear_alarm_btn);

        timSetBtn.setOnClickListener(clickListener);
        alarmReadBtn.setOnClickListener(clickListener);
        alarmClearBtn.setOnClickListener(clickListener);

        alarmLv = findViewById(R.id.alarm_lv);
        alarmListAdapter = new AlarmListAdapter(this, alarmBeanArrayList);
        alarmLv.setAdapter(alarmListAdapter);

        //初始化蓝牙
        BleManager.getInstance().setBleInterface(bleInterface);
        BleManager.getInstance().enableBluetooth(this);

        //时间更新线程
        PriorityRunnable runnable = new PriorityRunnable(PriorityType.NORMAL, timeUpdateRunnable);
        LocalThreadPools.getInstance().getExecutorService().execute(runnable);

        //测试。。。
        for (int i = 0; i < 10; i++) {
            AlarmBean bean = new AlarmBean();
            bean.setId(i);
            bean.setWeek(6);
            bean.setHour(12);
            bean.setMin(24);

            alarmBeanArrayList.add(bean);
        }
        alarmListAdapter.notifyDataSetChanged();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        activityExitFlag = true;

        BleManager.getInstance().exit();
        LocalThreadPools.getInstance().close();
        removeALLActivity();
        System.exit(0);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.toobar_menu, menu);
        menuItemConn = menu.findItem(R.id.item_conn);
        menuItemConn.setOnMenuItemClickListener(new MenuItem.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem menuItem) {
                connect();
                return true;
            }
        });

        return super.onCreateOptionsMenu(menu);
    }

    private void connect() {
        if (!Permission.isLocServiceEnable(this)) {
            ViseLog.e("定位未开启");
            openGPSSettings();
        } else {
            BluetoothLeDevice bleDeivce = BleManager.getInstance().getBleDeivce();
            if (!ViseBle.getInstance().isConnect(bleDeivce)) {
                //未连接，开启扫描设备
                deviceDialog.show();
                ViseBle.getInstance().startScan(scanCallback);
            } else {
                ViseBle.getInstance().disconnect(bleDeivce);
            }
        }
    }

    private void setMtu() {
        ViseLog.d("修改蓝牙设备MTU");
        BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter bluetoothAdapter = bluetoothManager.getAdapter();
        BluetoothDevice device = bluetoothAdapter.getRemoteDevice(BleManager.getInstance().getBleDeivce().getAddress());

        device.connectGatt(this, false, new BluetoothGattCallback() {
            @Override
            public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                super.onServicesDiscovered(gatt, status);

            }

            @Override
            public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
                super.onConnectionStateChange(gatt, status, newState);
                if (newState == BluetoothGatt.STATE_CONNECTED) {
                    ViseLog.d("修改Ble-MTU");
                    gatt.requestMtu(BleConstants.SEND_LEN_MAX);
                } else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
                    ViseLog.d("断开Ble连接，开始VisBle连接");
                    //开始连接
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            ViseBle.getInstance().connect(BleManager.getInstance().getBleDeivce(), connectCallback);
                        }
                    });
                }
            }

            @Override
            public void onMtuChanged(BluetoothGatt gatt, int mtu, int status) {
                super.onMtuChanged(gatt, mtu, status);
                ViseLog.d("修改Ble-MTU成功");
                BleManager.getInstance().setChangeMTU(true);
                gatt.disconnect();
            }
        });

    }

    private void openGPSSettings() {
        LayoutInflater factory = LayoutInflater.from(this);  //图层模板生成器句柄
        final View DialogView = factory.inflate(R.layout.dialog_gps_permission, null);  //用sname.xml模板生成视图模板
        androidx.appcompat.app.AlertDialog gps_dialog = new androidx.appcompat.app.AlertDialog.Builder(this)
                .setView(DialogView)
                // 拒绝, 退出应用
                .setNegativeButton("取消",
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {

                            }
                        })

                .setPositiveButton("设置",
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                //跳转GPS设置界面
                                Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                                startActivityForResult(intent, BleConstants.CODE_GPS_REQUEST);
                            }
                        })

                .setCancelable(false)
                .show();

        Button btnPositive = gps_dialog.getButton(android.app.AlertDialog.BUTTON_POSITIVE);
        Button btnNegative = gps_dialog.getButton(DialogInterface.BUTTON_NEGATIVE);

        btnPositive.setAllCaps(false);
        btnNegative.setAllCaps(false);

        int btnColor = ContextCompat.getColor(this, R.color.black);
        btnPositive.setTextColor(btnColor);
        btnNegative.setTextColor(btnColor);

    }

    private void toastInfoUiThread(String info) {
        Context context = this.getApplicationContext();
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(context, info, Toast.LENGTH_LONG).show();
            }
        });
    }

}