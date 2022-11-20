import VernierShieldCommunication as avc

##
## ANALOG testing. I can achieve 1.5kHz on one channel.
##
def anaogTesting(vernier_shield_object):
    #
    # Set up the Shield for 1kHz sampling
    if vernier_shield_object.set_sample_rate(avc.SampleRates.S_10Hz):
        print('--sample rate set--')
    #
    # Start right away.
    if vernier_shield_object.set_trigger_condition(condition=avc.Trigger.IMMEDIATE):
        print('--trigger set--')
    #
    # Collect 20 Points
    if vernier_shield_object.set_stop_condition(20):
        print('--stop set--')

    #
    # See what the current status is
    if vernier_shield_object.get_status([avc.Sources.ANA105]):
        vernier_shield_object.run_loop(1.0)  # wait for response to status

    #
    # Sync clocks
    if vernier_shield_object.sync_clocks():
        print("clocks synced")

    #
    # Arm and wait for data. (we chose IMMEDIATE triggering)
    if vernier_shield_object.arm_data([avc.Sources.ANA105]):
        print("begin...")
        vernier_shield_object.run_loop(5.0)  # wait for data to pour in for 5 sec or until stop condition is met.

##
## DIGITAL testing.
##
def digitalTesting(vernier_shield_object):
    digData = []

    def myBlobHandler(seq, data, deltime, src):
        #print(f"**{src:2d}, {seq:5d}, {data:5d}, {deltime:10f}") # f strings
        digData.append([data, deltime])  # just store the data for later retrieval

    vernier_shield_object.setDataHandler(avc.Sources.DIG1, myBlobHandler)

    #
    # Trigger on any transition.
    if vernier_shield_object.set_digital_trigger([avc.Trigger.LOW_2_HIGH]):
        print('--trigger set--')

    #
    # Arm and wait for data. (we chose IMMEDIATE triggering)
    if vernier_shield_object.arm_data([avc.Sources.DIG1]):
        print("begin...")
        vernier_shield_object.run_loop(5.0)  # wait for data to pour in for 5.0 seconds

    vernier_shield_object.setDataHandler(vernier_shield_object.dig01_handler)
    return digData


def main():
    vso = avc.VernierShieldCommunication(wait=1)
    if vso.wait_for_start():
        print("GOOD")
    else:
        print("BAD")

    if vso.get_version():
        vso.run_loop(1.0)

    print("--- Begin data taking ---")

    anaogTesting(vso)
#    someData = digitalTesting(vso)
#    print(f"{someData}")

    print("done.")
    vso.close()


if __name__ == "__main__":
    main()
