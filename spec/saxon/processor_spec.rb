RSpec.describe Saxon::Processor do
    subject(:processor) { Saxon::Processor.new }
    
    describe :new do
        it "can be initialized with no arguments" do
            expect{Saxon::Processor.new}.not_to raise_error
        end
    end

    describe :newDocumentBuilder do
        it "returns a DocumentBuilder" do
            expect(processor.newDocumentBuilder).to be_a(Saxon::DocumentBuilder)
        end
    end
end