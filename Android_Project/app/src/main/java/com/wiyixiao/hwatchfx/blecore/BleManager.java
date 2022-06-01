package com.wiyixiao.hwatchfx.blecore;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;

import com.vise.baseble.ViseBle;
import com.vise.baseble.callback.IBleCallback;
import com.vise.baseble.common.PropertyType;
import com.vise.baseble.core.BluetoothGattChannel;
import com.vise.baseble.core.DeviceMirror;
import com.vise.baseble.exception.BleException;
import com.vise.baseble.model.BluetoothLeDevice;
import com.vise.baseble.utils.BleUtil;
import com.vise.log.ViseLog;
import com.wiyixiao.hwatchfx.utils.Utils;

import java.util.LinkedList;
import java.util.Queue;
import java.util.UUID;

public class BleManager {
    private static BleManager instance = null;

    private Context context;

    /**
     * MTU修改标志
     */
    private boolean changeMTU = false;

    /**
     * 发送接收长度
     */
    public int recvLen;
    public int sendLen;

    /**
     * UUID
     */
    private UUID serviceUUID;
    private UUID characteristicWriteUUID;
    private UUID characteristicReadNotifyUUID;

    private BluetoothLeDevice bleDeivce;
    private DeviceMirror bleDeviceMirror;

    /**
     * IBleInterface 状态监听器
     */
    private IBleInterface bleInterface;


    /**
     * ++++++++++++++++++++++++++++++++SET/GET+++++++++++++++++++++++++++++++++++++
     */
    public void setBleInterface(IBleInterface bleInterface) {
        this.bleInterface = bleInterface;
    }

    public void setBleDeivce(BluetoothLeDevice bleDeivce) {
        this.bleDeivce = bleDeivce;
    }

    public BluetoothLeDevice getBleDeivce() {
        return bleDeivce;
    }

    public IBleInterface getBleInterface() {
        return bleInterface;
    }

    public void setChangeMTU(boolean changeMTU) {
        this.changeMTU = changeMTU;
    }

    /**
     * ++++++++++++++++++++++++++++++++Public+++++++++++++++++++++++++++++++++++++
     */
    public static BleManager getInstance() {
        if (instance == null) {
            instance = new BleManager();
        }

        return instance;
    }

    public void init(Context context) {
        this.context = context;

        //蓝牙相关配置修改
        ViseBle.config()
                .setScanTimeout(6000)//扫描超时时间，这里设置为永久扫描
                .setConnectTimeout(10 * 1000)//连接超时时间
                .setOperateTimeout(5 * 1000)//设置数据操作超时时间
                .setConnectRetryCount(10)//设置连接失败重试次数
                .setConnectRetryInterval(1000)//设置连接失败重试间隔时间
                .setOperateRetryCount(3)//设置数据操作失败重试次数
                .setOperateRetryInterval(1000)//设置数据操作失败重试间隔时间
                .setMaxConnectCount(3);//设置最大连接设备数量
        ViseBle.getInstance().init(context);
    }

    public void enableBluetooth(Activity activity) {
        if (!BleUtil.isBleEnable(context)) {
            BleUtil.enableBluetooth(activity, 1);
        } else {
            boolean isSupport = BleUtil.isSupportBle(context);
            boolean isOpenBle = BleUtil.isBleEnable(context);
            if (isSupport) {
                ViseLog.i("设备支持蓝牙");
            } else {
                ViseLog.i("设备不支持蓝牙");
            }
            if (isOpenBle) {
                if (bleInterface != null) {
                    bleInterface.toastMsg("Bluetooth on");
                }
            } else {
                if (bleInterface != null) {
                    bleInterface.toastMsg("Bluetooth off");
                }
            }
        }
    }

    /**
     * 获取设备UUID
     */
    public void getDeviceUUID() {
        String uuid_temp;
        bleDeviceMirror = ViseBle.getInstance().getDeviceMirror(bleDeivce);
        //绑定数据通道
        for (final BluetoothGattService gattservice : bleDeviceMirror.getBluetoothGatt().getServices()
        ) {
            ViseLog.i(gattservice.getUuid().toString());
            uuid_temp = gattservice.getUuid().toString();

            switch (uuid_temp) {
                case BleUUID.service_uuid: {
                    serviceUUID = gattservice.getUuid();
                    for (final BluetoothGattCharacteristic gattCharacteristic : gattservice.getCharacteristics()
                    ) {
                        uuid_temp = gattCharacteristic.getUuid().toString();
                        if (uuid_temp.equals(BleUUID.service_write_uuid)) {
                            //找到写入数据服务
                            ViseLog.i("Write: %s", uuid_temp);
                            characteristicWriteUUID = gattCharacteristic.getUuid();
                        }else if(uuid_temp.equals(BleUUID.service_read_notify_uuid)){
                            //找到读取通知服务
                            ViseLog.i("Read and Notify: %s", uuid_temp);
                            characteristicReadNotifyUUID = gattCharacteristic.getUuid();
                        }
                    }
                }
                break;
                default:
                    break;
            }
        }

        //绑定写入通道
        bindChannel(bleDeviceMirror,
                PropertyType.PROPERTY_WRITE,
                serviceUUID,
                characteristicWriteUUID,
                null,
                writeCallback);

//        //修改MTU长度
//        if (!BleConstants.changeMtuEnable) return;
//        changeMTU = bleDeviceMirror.getBluetoothGatt().requestMtu(BleConstants.SEND_LEN_MAX);
//        if (changeMTU) {
//            ViseLog.e("修改MTU成功");
//            if (bleInterface != null) {
//                bleInterface.toastMsg("修改MTU成功");
//            }
//        }
        //绑定接收通道
        bindChannel(bleDeviceMirror,
                PropertyType.PROPERTY_NOTIFY,
                serviceUUID,
                characteristicReadNotifyUUID,
                null,
                notifyCallback);
        bleDeviceMirror.registerNotify(false);
    }

    /**
     * 绑定通道
     */
    private void bindChannel(DeviceMirror mirror, PropertyType propertyType, UUID serviceUUID, UUID
            characteristicUUID, UUID descriptorUUID, IBleCallback callback) {
        if (mirror != null) {
            BluetoothGattChannel bluetoothGattChannel = new BluetoothGattChannel.Builder()
                    .setBluetoothGatt(mirror.getBluetoothGatt())
                    .setPropertyType(propertyType)
                    .setServiceUUID(serviceUUID)
                    .setCharacteristicUUID(characteristicUUID)
                    .setDescriptorUUID(descriptorUUID)
                    .builder();
            mirror.bindChannel(callback, bluetoothGattChannel);
        }
    }


    /**
     * 按字符串发送
     *
     * @param data 字符串
     */
    public void bleWriteData(String data) {
        try {
            bleWriteData(data.getBytes("GBK"));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 按字节发送
     *
     * @param data 字节数组
     */
    public void bleWriteData(byte[] data) {
        if (bleDeviceMirror != null) {
            sendLen += data.length;
            bleDeviceMirror.writeData(data);
        }
    }

    /**
     * 大于20字节，分包发送
     *
     * @param data 数据
     */
    public void bleWritePack(byte[] data) {
        if (dataInfoQueue != null) {
            dataInfoQueue.clear();
            dataInfoQueue = splitPacketForByte(data);
            new Handler(Looper.getMainLooper()).post(new Runnable() {
                @Override
                public void run() {
                    send();
                }
            });
        }
    }

    /**
     * 读取数据
     */
    private void bleReadData() {
        if (bleDeviceMirror != null) {
            BluetoothGattChannel bluetoothGattChannel = new BluetoothGattChannel.Builder()
                    .setBluetoothGatt(bleDeviceMirror.getBluetoothGatt())
                    .setPropertyType(PropertyType.PROPERTY_READ)
                    .setServiceUUID(serviceUUID)
                    .setCharacteristicUUID(characteristicReadNotifyUUID)
                    .setDescriptorUUID(null)
                    .builder();
            bleDeviceMirror.bindChannel(new IBleCallback() {
                @Override
                public void onSuccess(byte[] data, BluetoothGattChannel bluetoothGattChannel, BluetoothLeDevice bluetoothLeDevice) {
                    ViseLog.d(Utils.bytesToHex(data));
                }

                @Override
                public void onFailure(BleException exception) {

                }
            }, bluetoothGattChannel);

            bleDeviceMirror.readData();
        }
    }

    public void exit() {
        bleInterface = null;
        ViseBle.getInstance().clear();
    }

    /**+++++++++++++++++++++++++++++++++++++++Callback++++++++++++++++++++++++++++++++++++++*/
    /**
     * 写入回调
     */
    private IBleCallback writeCallback = new IBleCallback() {

        @Override
        public void onSuccess(byte[] data, BluetoothGattChannel bluetoothGattChannel, BluetoothLeDevice bluetoothLeDevice) {

        }

        @Override
        public void onFailure(BleException exception) {

        }
    };

    /**
     * 可通知回调
     */
    private IBleCallback notifyCallback = new IBleCallback() {
        @Override
        public void onSuccess(byte[] data, BluetoothGattChannel bluetoothGattChannel, BluetoothLeDevice bluetoothLeDevice) {
            DeviceMirror deviceMirror = bleDeviceMirror;
            if (deviceMirror != null) {
                ViseLog.i("send ok...");
                deviceMirror.setNotifyListener(bluetoothGattChannel.getGattInfoKey(), recvCallback);
            }
        }

        @Override
        public void onFailure(BleException exception) {

        }
    };

    /**
     * 接收数据回调
     */
    private IBleCallback recvCallback = new IBleCallback() {
        @Override
        public void onSuccess(byte[] data, BluetoothGattChannel bluetoothGattChannel, BluetoothLeDevice bluetoothLeDevice) {
            recvLen += data.length; //计算接收长度

            if (bleInterface != null) {
                bleInterface.recvCall(data);
            }
        }

        @Override
        public void onFailure(BleException exception) {

        }
    };

    /**
     * +++++++++++++++++++++++++++++++++++++++Private++++++++++++++++++++++++++++++++++++++
     */
    //发送队列，提供一种简单的处理方式，实际项目场景需要根据需求优化
    private Queue<byte[]> dataInfoQueue = new LinkedList<>();

    private void send() {
        if (dataInfoQueue != null && !dataInfoQueue.isEmpty()) {
            if (dataInfoQueue.peek() != null && bleDeviceMirror != null) {
                bleWriteData(dataInfoQueue.poll());
            }
            if (dataInfoQueue.peek() != null) {
                new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        send();
                    }
                }, 5);
            }
        }
    }

    /**
     * 数据分包
     *
     * @param data 数据
     * @return Queue<byte [ ]>
     */
    private Queue<byte[]> splitPacketForByte(byte[] data) {
        Queue<byte[]> dataInfoQueue = new LinkedList<>();
        int pack_len = changeMTU ? (BleConstants.SEND_LEN_MAX - 10) : BleConstants.SEND_LEN_MIN;
        if (data != null) {
            int index = 0;
            do {
                byte[] surplusData = new byte[data.length - index];
                byte[] currentData;
                System.arraycopy(data, index, surplusData, 0, data.length - index);
                if (surplusData.length <= pack_len) {
                    currentData = new byte[surplusData.length];
                    System.arraycopy(surplusData, 0, currentData, 0, surplusData.length);
                    index += surplusData.length;
                } else {
                    currentData = new byte[pack_len];
                    System.arraycopy(data, index, currentData, 0, pack_len);
                    index += pack_len;
                }
                dataInfoQueue.offer(currentData);
            } while (index < data.length);
        }
        return dataInfoQueue;
    }

}
