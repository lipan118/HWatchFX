package com.wiyixiao.hwatchfx.blecore;

import androidx.annotation.StringDef;

@StringDef({BleUUID.service_uuid,
            BleUUID.service_write_uuid,
            BleUUID.service_read_notify_uuid,})
public @interface BleUUID {

    String service_uuid = "0000fff0-0000-1000-8000-00805f9b34fb";

    String service_write_uuid = "0000fff2-0000-1000-8000-00805f9b34fb";

    String service_read_notify_uuid = "0000fff1-0000-1000-8000-00805f9b34fb";

}
