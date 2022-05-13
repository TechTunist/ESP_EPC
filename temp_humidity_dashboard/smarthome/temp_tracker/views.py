from turtle import title
from django.shortcuts import render
from django.http import HttpResponse
from .models import DH11Sensor
import plotly.express as px
import plotly.graph_objs as go
from plotly.subplots import make_subplots

import serial


def home(request):

    arduino_port = "COM5"
    baud = 9600
    file_name = "serial_data.csv"
    samples = 10

    try:

        ser = serial.Serial(arduino_port, baud)
        print("Connected to Arduino port", arduino_port)

        line = 0

        while line <= samples:

            print("Line " + str(line) + ": writing...")
            
            get_data = ser.readline().decode('utf-8')
            
            # extract humidity and temperature from serial port data stream
            humidity, temp, location = round(float(get_data.split(",")[0]), 1), round(float(get_data.split(",")[1]), 2), get_data.split(",")[2]

            # create instance of database model
            record = DH11Sensor()

            # add each row to the database
            record.humidity = humidity
            record.temperature = temp
            record.location = location
            record.save()
        
            print("Humidity: ", humidity, "\nTemperature: ", temp, "\nLocation: ", location)
            
            # write the data to a csv file
            with open(file_name, "a") as file:
                file.write(get_data)
                line += 1

        # close the serial port after gathering data
        ser.close()

        print("Data collection complete")

    except serial.SerialException:
        print("\nSerial Connection Failed\n")

    
    # access database fields
    database = DH11Sensor.objects.all()

    # plot plotly express (px) chart
    # x_data = [records.time_of_record for records in database]
    # y_data = [records.temperature for records in database]

    # fig = px.line(
    #     x=x_data,
    #     y=y_data,
    #     title="Temperature at Office Desk",
    #     labels={"x": "Time", "y": "Temperature"}
    # )

    # plotly.graph_objects
    fig1 = make_subplots(specs=[[{"secondary_y": True}]])

    fig1.add_trace(
        go.Scatter(x = [records.time_of_record for records in database],
                y = [records.temperature for records in database],
                name="Temperature"), secondary_y = False
    )

    fig1.add_trace(
        go.Scatter(x = [records.time_of_record for records in database],
                y = [records.humidity for records in database],
                name = "Humidity"), secondary_y = True
    )

    # add figure title
    fig1.update_layout(
        title_text = "Temperature / Humidity"
    )

    # set x-axis title
    fig1.update_xaxes(
        title_text = "Time of Measurement"
    )

    # set primary y-axis title
    fig1.update_yaxes(
        title_text = "Temperature",
        secondary_y = False
    )

    # set secondary y-axis title
    fig1.update_yaxes(
        title_text = "Temperature",
        secondary_y = False
    )

    chart1 = fig1.to_html()

    ########################################
        # plotly.graph_objects
    fig2 = make_subplots(specs=[[{"secondary_y": True}]])

    fig2.add_trace(
        go.Scatter(x = [records.time_of_record for records in database],
                y = [records.temperature for records in database],
                name="Temperature"), secondary_y = False
    )

    fig2.add_trace(
        go.Scatter(x = [records.time_of_record for records in database],
                y = [records.humidity for records in database],
                name = "Humidity"), secondary_y = True
    )

    # add figure title
    fig2.update_layout(
        title_text = "Temperature / Humidity"
    )

    # set x-axis title
    fig2.update_xaxes(
        title_text = "Time of Measurement"
    )

    # set primary y-axis title
    fig2.update_yaxes(
        title_text = "Temperature",
        secondary_y = False
    )

    # set secondary y-axis title
    fig2.update_yaxes(
        title_text = "Temperature",
        secondary_y = False
    )

    chart2 = fig2.to_html()

    context = {"chart1": chart1,
                "chart2": chart2}

    return render(request, 'home.html', context)
    

####### Add a live stream from the sensors on separate view #######

def liveStream(request):

    context = {}

    return render(request, 'live.html', context)
