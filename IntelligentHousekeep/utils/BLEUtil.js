/**
 * 蓝牙跳绳基类，用于蓝牙相关通信均可继承
 */
import {
    inArray,	// 展示搜索到的设备列表时去重
    uuid2Mac, // 用于统一ios与android设备端展示的deviceId格式为：xx:xx:xx:xx:xx 
    deviceNameFilter,
    utf8to16,
    hexToString,
    ab2hex,
    str2ab,
  } from "./util.js";
  
  class BLEController {
    // 自动关闭定时器
    findTimer = null;
    // 蓝牙适配器开启状态
    static adapterOpend = false;
    // 扫描设备状态
    startDiscovery = false;
    // 蓝牙连接状态
    connectStatus = false;
    // 蓝牙扫描自动结束时间2min
    #timeout = 2 * 60 * 1000;
    // 蓝牙通信超时时间5min
    #notifyTimeout = 5 * 60 * 1000;
    // 蓝牙搜索是否超时
    deviceDiscoveryTimeout = false;
    // 蓝牙设备ID，注意：ios设置对应deviceId为设备uuid，安卓及开发者工具上连接的蓝牙为mac地址
    deviceId;
    deviceName;
    // 设备mac地址（统一编码处理）
    deviceMac;
    // 设备列表, [{deviceMac:设备mac地址， deviceId:设备ID,deviceName:设备名称，...}]
    deviceList = [];
    // 蓝牙服务特征队列
    characteristicStack = [];
    // 蓝牙消息队列
    msgStack = [];
    // 蓝牙通信serviceId
    serviceId = "";
    constructor(context) {
      if (context) {
        this.deviceId = context.deviceId;
        this.deviceMac = context.deviceMac;
        this.connectStatus = false;
      }
    }
    /**
     *  1.初始化蓝牙模块
     */
    openBluetoothAdapter() {
      const _this = this;
      if (BLEController.adapterOpend) {
        console.log("蓝牙适配器已打开,请勿重复操作------》");
        return;
      }
      wx.openBluetoothAdapter({
        mode: "central",
        success(res) {
          BLEController.adapterOpend = true;
          console.log("蓝牙适配器打开成功-----------------》");
        },
        fail(res) {
          BLEController.adapterOpend = false;
          _this.BLEFail(res);
          console.log("蓝牙适配器打开失败-----------------》", res.errMsg);
        },
      });
    }
    /**
     *  2.扫描蓝牙设备(绑定蓝牙，连接蓝牙通用)
     * @param {Array} options.keywords 蓝牙名称筛选关键字
     * @param {string} options.deviceId 可选参数，蓝牙设备id，连接用
     */
    startBluetoothDevicesDiscovery(options) {
      // ---------省略---------------》
      if (this.startDiscovery) {
        console.log("已开启蓝牙扫描，勿重复开启-----------》");
        return;
      } else {
        this.startDiscovery = true;
        wx.startBluetoothDevicesDiscovery({
          allowDuplicatesKey: false,
          success: (res) => {
            this.onBluetoothDeviceFound(options);
            console.log("开始扫描蓝牙设备-----------------》");
          },
          fail: (res) => {
            this.startDiscovery = false;
          },
        });
      }
    }
    /**
     *  2.1 监听搜索到新设备
     * @param {Array} options.keywords 蓝牙名称筛选关键字,来自startBluetoothDevicesDiscovery调用
     * @param {string} options.deviceId
     */
    onBluetoothDeviceFound(options) {
      let { keywords } = options;
      // 超时自动结束
      this.findTimer = setTimeout(() => {
        clearTimeout(this.findTimer);
        if (!this.connectStatus) {
          console.log("蓝牙扫描超时，自动关闭任务-------------》");
          this.deviceDiscoveryTimeout = true;
          this.startDiscovery = false;
          wx.stopBluetoothDevicesDiscovery();
        }
      }, this.#timeout);
      // 监听扫描
      wx.onBluetoothDeviceFound((res) => {
        let devices = res.devices;
        devices.forEach((device) => {
          if (!device.name && !device.localName) {
            return;
          }
          // 获取设备MAC地址，并根据关键字过滤
          let systemInfo = wx.getSystemInfoSync();
          let iosDevice = systemInfo.system.toLowerCase().indexOf("ios") > -1;
          let deviceMac = iosDevice
            ? uuid2Mac(device.advertisData)
            : device.deviceId;
          if (keywords && keywords.length > 0) {
            if (deviceNameFilter(device.name, keywords)) {
              const foundDevices = this.deviceList;
  
              const idx = inArray(foundDevices, "deviceMac", deviceMac);
              let devicesInner = [...this.deviceList];
              if (idx === -1) {
                device.deviceMac = deviceMac;
                devicesInner[foundDevices.length] = device;
                this.deviceList = devicesInner;
                console.log("发现蓝牙设备并更新列表-----------》", deviceMac);
              }
            }
          }
        });
      });
    }
    /**
     * 3. 直接连接蓝牙
     * @param {string} options.deviceId 蓝牙设备id，连接用
     */
    createBLEConnection(options) {
      let { deviceId } = options;
      this.deviceId = deviceId;
      // 如果开启扫描时适配器还没启动，轮询等待
      const _this = this;
      if (!BLEController.adapterOpend) {
        // ---------省略---------------》
      }
      if (this.connectStatus) {
        wx.closeBLEConnection({
          deviceId: deviceId,
        });
      }
      let timeout = this.#timeout;
      console.log("开始连接蓝牙------------》", deviceId);
      this.stopBLEDevicesTask();
      wx.createBLEConnection({
        deviceId: deviceId,
        timeout: timeout,
        success: (res) => {
          console.log("蓝牙连接成功----------》", deviceId);
          this.connectStatus = true;
          this.onBLEConnectionStateChange();
          this.getBLEDeviceServices();
        },
        fail: (res) => {
          this.connectStatus = false;
          console.log("连接失败-------》", res.errMsg);
        },
      });
    }
    /**
     * 3.1 获取蓝牙低功耗设备所有服务
     * @param {string} deviceId 蓝牙设备Id，来自createBLEConnection调用
     */
    getBLEDeviceServices() {
      wx.getBLEDeviceServices({
        deviceId: this.deviceId,
        success: (res) => {
          /**
           * 16 位 UUID 从对接文档中获取（注意都是0000开头，接着的4位数字为16进制的uuid,所有服务只有4位uuid不一样）
           * 跳绳主服务 0000xxxx-0000-1000-8000-00805f9b34fb （0xXXXX）
           */
          //  注意有多个服务，不同服务的操作不一样，单个服务只能执行单个操作，所以这里需要建立多个连接
          for (let i = 0; i < res.services.length; i++) {
            // 注意uuid的大小写
            if (
              res.services[i].isPrimary &&
              res.services[i].uuid == "0000xxxx-0000-1000-8000-00805F9B34FB"
            ) {
              this.getBLEDeviceCharacteristics(res.services[i].uuid);
              return;
            }
          }
        },
        fail: (res) => {
          console.log("服务获取失败------------->", res.errMsg);
        },
      });
    }
    /**
     * 3.2 获取蓝牙低功耗设备某个服务中所有特征 (characteristic)
     * @param {string} uuid 服务UUID，来自getBLEDeviceServices调用
     */
    getBLEDeviceCharacteristics(serviceId) {
      wx.getBLEDeviceCharacteristics({
        deviceId: this.deviceId,
        serviceId,
        success: (res) => {
          // 设备特征列表
          let characteristics = res.characteristics;
          for (let i = 0; i < characteristics.length; i++) {
            let item = characteristics[i];
            // 该特征是否支持 read 操作
            if (item.properties.read) {
              wx.readBLECharacteristicValue({
                deviceId: this.deviceId,
                serviceId,
                characteristicId: item.uuid,
              });
            }
            // 该特征是否支持 write 操作
            if (item.properties.write) {
              this.serviceId = serviceId;
              this.characteristicId = item.uuid;
              this.onConnectSuccess();
            }
            // 该特征是否支持 notify ,indicate操作 ，开启监听订阅特征消息
            if (item.properties.notify || item.properties.indicate) {
              wx.notifyBLECharacteristicValueChange({
                deviceId: this.deviceId,
                serviceId,
                characteristicId: item.uuid,
                state: true,
              });
              this.onBLECharacteristicValueChange();
            }
          }
        },
      });
    }
  
    /**
     * 3.4 监听蓝牙数据
     * @param
     */
    onBLECharacteristicValueChange() {
      wx.onBLECharacteristicValueChange((characteristic) => {
        let { characteristicId, value } = characteristic;
        const idx = inArray(this.characteristicStack, "uuid", characteristicId);
        let formatedValue = hexToString(ab2hex(value));
  
        if (
          this.msgStack.indexOf(formatedValue) < 0 ||
          this.msgStack.length > 2
        ) {
          this.msgStack.push(formatedValue);
          if (formatedValue.indexOf("#") > -1) {
            var dataValue = this.msgStack.join("");
            this.msgStack = [];
            // 消息事件
            this.onMsgValueChange(dataValue);
          }
        }
        if (idx === -1) {
          this.characteristicStack.push({
            uuid: characteristic.characteristicId,
            value: value,
          });
        } else {
          this.characteristicStack.splice(idx, 1, {
            uuid: characteristic.characteristicId,
            value: value,
          });
        }
      });
    }
  
    /**
     * 3.6 蓝牙状态变化监听
     */
    onBLEConnectionStateChange() {
      wx.onBLEConnectionStateChange((res) => {
        let { deviceId, connected } = res;
        this.connectStatus = connected;
        console.log("蓝牙状态变化 -------------》", connected, deviceId);
      });
    }
    /**
     * 4. 发送蓝牙指令。蓝牙指令超出20字符时需要截断多次发送
     * @param {string} cmdStr 蓝牙指令
     * @param {string} cmdName 蓝牙指令名称——可选用于打印调试
     */
    writeBLECharacteristicValue(cmdStr, cmdName) {
      console.log("发送蓝牙指令------------》", cmdStr, cmdName);
      var byteLen = cmdStr.length;
      var pos = 0;
      let loopCount = 0;
      // 消息超长分批处理
      for (let i = 0; i < byteLen; i += 20) {
        let buffer = str2ab(cmdStr.slice(pos, pos + 20));
        pos += 20;
        loopCount += 1;
        let param = {
          deviceId: this.deviceId,
          serviceId: this.serviceId,
          characteristicId: this.characteristicId,
          value: buffer,
        };
        console.log(`cyy:第${loopCount}次发送指令${cmdName}:`, param);
        wx.writeBLECharacteristicValue({
          ...param,
          success: function (res) {
            console.log("发送指令成功", cmdName);
          },
          fail: function (res) {
            console.warn("发送指令失败", cmdName, res);
          },
        });
      }
    }
    /**
     * 蓝牙错误拦截
     */
    BLEFail(res) {
      wx.hideLoading();
      if (res.errno == 103) {
        uni.showModal({
          title: "提示",
          content: "请先开启蓝牙权限",
          showCancel: false,
          success: function () {
            wx.openSetting();
          },
        });
      } else if (res.errCode === 10001) {
        uni.showModal({
          title: "提示",
          content: "请先打开手机蓝牙！",
          showCancel: false,
        });
      } else {
        console.log("cyy: 蓝牙错误---------》", res.errCode);
      }
      this.errorHandler();
    }
    /**
     * 停止蓝牙通信活动及监听
     */
    stopBLEDevicesTask() {
      // 停止扫描设备
      this.startDiscovery = false;
      wx.stopBluetoothDevicesDiscovery();
      // 关闭扫描新设备监听
      wx.offBluetoothDeviceFound();
      // 关闭数据监听
      wx.offBLECharacteristicValueChange();
      // 移除蓝牙低功耗连接状态改变事件的监听函数
      wx.offBLEConnectionStateChange();
    }
    /**
     * 停止所有蓝牙活动
     */
    closeBLE() {
      // 关闭线程
      if (this.findTimer) {
        clearTimeout(this.findTimer);
      }
      // 停止扫描
      this.stopBLEDevicesTask();
  
      // 断开连接
      if (this.deviceId) {
        wx.closeBLEConnection({
          deviceId: this.deviceId,
        });
        this.connectStatus = false;
      }
      // 关闭适配器
      BLEController.adapterOpend = false;
      wx.closeBluetoothAdapter();
    }
    /**
     * @override 蓝牙报错后处理
     */
    errorHandler() {}
    /**
     * @override 3.5 消息监听通知——实例化时重写
     */
    onMsgValueChange(dataValue) {}
    /**
     * @override 3.3连接成功处理函数——可通过继承重写
     */
    onConnectSuccess() {}
  }
  
  export default BLEController;
  
  