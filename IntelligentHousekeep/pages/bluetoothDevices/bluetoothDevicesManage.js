// pages/bluetoothDevices/bluetoothDevicesManage.js
Page({

    /**
     * 页面的初始数据
     */
    data: {
        deviceId:"",//设备ID
        deviceName:"",//设备名称
        iconList: [{
            icon: 'wifi',
            color: 'red',
            badge: 0,
            name: '设置WIFI',
            fun:'showSetWifi'
          }, {
            icon: 'creativefill',
            color: 'orange',
            badge: 1,
            name: '灯控',
            fun:'showLampManage'
          }, {
            icon: 'timefill',
            color: 'yellow',
            badge: 0,
            name: '获取在线时长',
            fun:'getOnlineTime'
          }],
          gridCol:3,
          skin: false
    },

    /**
     * 生命周期函数--监听页面加载
     */
    onLoad(options) {
        console.log(options)
        this.setData({deviceId:options.deviceId,deviceName:options.deviceName});
    },

    /**
     * 生命周期函数--监听页面初次渲染完成
     */
    onReady() {

    },

    /**
     * 生命周期函数--监听页面显示
     */
    onShow() {

    },

    //进入WIFI设置界面
    showSetWifi(){
        var deviceId= this.data.deviceId;
        var deviceName=this.data.deviceName;
        wx.navigateTo({
          url: '/pages/bluetoothDevices/setWifi/setWifi?deviceId='+deviceId+'&deviceName='+deviceName,
        })
    },
    //进入灯控管理
    showLampManage(){

    },
    //获取设备在线时长
    getOnlineTime(){
        console.log(343534)
    },

    /**
     * 生命周期函数--监听页面隐藏
     */
    onHide() {

    },

    /**
     * 生命周期函数--监听页面卸载
     */
    onUnload() {

    },
    
})